#include "csapp.h"

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

typedef struct {
    char *hostname;
    char *port;
    char *uri;
} http_object_identifier;

typedef struct {
    http_object_identifier *object_identifier;
    size_t content_length;
    char *content;
} http_object;

typedef struct cache_list {
    http_object *cache_object;
    struct cache_list *prev;
    struct cache_list *next;
} cache_list;

struct {
    cache_list *lru;
    cache_list *mru;
    size_t size;
} cache = {NULL, NULL, 0};

sem_t mutex;

/* You won't lose style points for including this long line in your code */
// static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";

// Proxy Functionality
void *handle_client(void *client_fdp);
int echo_request(int client_fd, http_object_identifier **object_identifier);
void echo_response(int client_fd, int server_fd, http_object_identifier *object_identifier);

// Proxy Internals
void parse_header(char *header, char *hostname, char *port, char *uri);
size_t parse_header_line(char *buf);
size_t parse_response_for_content_length(char *buf);

// Utility
char *find_index(char *string, char token);
void client_error(char *msg);


// Cache Management
void cache_push(http_object_identifier *object_identifier, size_t content_length, char *content);
http_object *cache_fetch(char *hostname, char *port, char *uri);
void assemble_response(int client_fd, http_object *cached_object);

// Cache Internals
cache_list *cache_lookup(char *hostname, char *port, char *uri);
void cache_update(cache_list *used);
void cache_evict();

// Cache Utility
http_object_identifier *create_http_object_identifier(char *hostname, char *port, char *uri);
cache_list *create_cache_element(http_object_identifier *object_identifier, size_t content_length, char *content);
void free_cache_element(cache_list *cache_element);





/* ============================================
                PROXY LOGIC
   ============================================ */

int main(int argc, char *argv[])
{
    int listen_fd, *client_fdp;
    pthread_t tid;
    struct sockaddr_storage client_addr;
    socklen_t client_len = sizeof(client_addr);
    char client_port[MAXLINE], client_hostname[MAXLINE];

    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }

    // Listen for connections to <port>
    Sem_init(&mutex, 0, 1);
    listen_fd = Open_listenfd(argv[1]);
    while (1) {
        // Wait and accept connections
        client_fdp = Malloc(sizeof(int));
        *client_fdp = Accept(listen_fd, (SA *)&client_addr, &client_len);

        Getnameinfo((SA *) &client_addr, client_len, client_hostname, MAXLINE, client_port, MAXLINE, 0);
        // printf("Accepted connection from %s:%s\n", client_hostname, client_port);

        Pthread_create(&tid, NULL, handle_client, client_fdp);
    }

    return 0;
}

void *handle_client(void *client_fdp) {
    int server_fd;
    http_object_identifier *object_identifier;
    int client_fd = * ((int *) client_fdp);
    Pthread_detach(pthread_self());
    Free(client_fdp);
    
    // Echo client request to server
    server_fd = echo_request(client_fd, &object_identifier);
    if (server_fd == -1) { // Cache hit
        Close(client_fd);
        return NULL;
    }        

    // Echo server reponse to client
    echo_response(client_fd, server_fd, object_identifier);

    Close(client_fd);
    Close(server_fd);
    return NULL;
}


// ----------- HANDLE CLIENT REQUEST -----------

int echo_request(int client_fd, http_object_identifier **object_identifier) {
    int server_fd;
    http_object *cached_object;
    rio_t client_rio;
    char buf[MAXLINE], hostname[MAXLINE], port[32], uri[MAXLINE];
    size_t line_size;


    // Get header
    Rio_readinitb(&client_rio, client_fd);
    if (Rio_readlineb(&client_rio, buf, MAXLINE) <= 0)
        client_error("Invalid HTTP Header");
    
    // Parse header
    parse_header(buf, hostname, port, uri);

    // Check cache *** LOCK ***
    P(&mutex);
    if ((cached_object = cache_fetch(hostname, port, uri)) != NULL) {
        // printf("Cache Hit: %s:%s%s\n", hostname, port, uri);
        // Return cached object
        assemble_response(client_fd, cached_object);
        V(&mutex);
        return -1;
    }
    V(&mutex);

    // Fill in identifying information
    *object_identifier = create_http_object_identifier(hostname, port, uri);

    // Connect to host server
    // printf("Connecting to %s:%s\n", hostname, port);
    server_fd = Open_clientfd(hostname, port);

    // Put HTTP/1.0 Header
    sprintf(buf, "GET %s HTTP/1.0\r\n", uri);
    Rio_writen(server_fd, buf, strlen(buf));

    // Fill in required fields

    // 1. HOST
    sprintf(buf, "Host: %s\r\n", hostname);
    Rio_writen(server_fd, buf, strlen(buf));

    // 2. CONNECTION
    sprintf(buf, "Connection: close\r\n");
    Rio_writen(server_fd, buf, strlen(buf));
    
    // 3. PROXY CONNECTION
    sprintf(buf, "Proxy-Connection: close\r\n");
    Rio_writen(server_fd, buf, strlen(buf));

    // Copy client request into server request line by line
    while (Rio_readlineb(&client_rio, buf, MAXLINE) > 0) {
        line_size = parse_header_line(buf);
        if (line_size == 0) 
            continue;
        Rio_writen(server_fd, buf, line_size);
        if (line_size == 2)
            break;
    }

    return server_fd;
}

void parse_header(char *header, char *hostname, char *port, char *uri) {
    char *temp;
    
    // Get method
    temp = header;
    header = find_index(header, ' ');
    *(header++) = '\0';

    // Verify method is GET
    if (strcmp(temp, "GET") != 0)
        client_error("Invalid Method (Only Accepting GET)");

    // Skip http://
    header = find_index(header, '/');
    header += 2;

    // Get hostname
    temp = header;
    header = find_index(header, ':');
    *(header++) = '\0';
    strcpy(hostname, temp);

    // Get port
    temp = header;
    header = find_index(header, '/');
    *header = '\0';
    strcpy(port, temp);

    // Get URI
    *header = '/';
    temp = header;
    header = find_index(header, ' ');
    *header = '\0';
    strcpy(uri, temp);
    return;
}

size_t parse_header_line(char *buf) {
    char *temp;

    // Check for empty line
    if (strcmp(buf, "\r\n") == 0)
        return 2;

    // Check for required headers (ignore them)
    temp = find_index(buf, ':');
    *temp = '\0';
    
    if ((strcmp(buf, "Host") == 0) || (strcmp(buf, "Connection") == 0) || (strcmp(buf, "Proxy-Connection") == 0)) {
        return 0;
        *temp = ':';
    }

    // Otherwise
    *temp = ':';
    return strlen(buf);
}

// ----------- HANDLE SERVER RESPONSE -----------

void echo_response(int client_fd, int server_fd, http_object_identifier *object_identifier) {
    rio_t server_rio;
    size_t n_read, content_length, content_read, total_read, temp;
    char buf[MAXLINE], cache_buf[MAX_OBJECT_SIZE], *content;
    Rio_readinitb(&server_rio, server_fd);

    content_length = 0;
    content = NULL;

    // Copy response header
    n_read = Rio_readlineb(&server_rio, buf, MAXLINE);
    Rio_writen(client_fd, buf, n_read);
    memcpy(cache_buf, buf, n_read);
    total_read = n_read;

    // Copy headers and look for content length
    while ((n_read = Rio_readlineb(&server_rio, buf, MAXLINE)) > 0) {
        temp = parse_response_for_content_length(buf);
        if (temp > 0) 
            content_length = temp;
        
        Rio_writen(client_fd, buf, n_read);
        memcpy(cache_buf + total_read, buf, n_read);
        total_read += n_read;
        if (strcmp(buf, "\r\n") == 0){ // Get out of header loop
            break;
        }
    }

    // If the object doesn't exceed MAX_OBJECT_SIZE, save it
    if ((total_read + content_length) <= MAX_OBJECT_SIZE) {
        content = (char *) Malloc(sizeof(char) * (total_read + content_length));
        memcpy(content, cache_buf, total_read);
    }

    // Copy body until all content is read
    content_read = 0;
    while (n_read < content_length) {
        n_read = Rio_readlineb(&server_rio, buf, MAXLINE);
        if (n_read == 0)
            break;
        
        Rio_writen(client_fd, buf, n_read);
        if (content) {
            memcpy(content + total_read, buf, n_read);
        }
        content_read += n_read;
        total_read += n_read;
    }
    // Push to cache *** LOCK ***
    if (content) {
        P(&mutex);
        cache_push(object_identifier, total_read, content);
        V(&mutex);
    }

    return;
}

size_t parse_response_for_content_length(char *buf) {
    char *temp, *end;
    size_t content_length;

    // Check for empty line
    if (strcmp(buf, "\r\n") == 0)
        return 0;

    // Check for content length header
    temp = find_index(buf, ':');
    *temp = '\0';

    if (strcmp(buf, "Content-length") == 0) {
        *(temp++) = ':';
        end = find_index(temp, '\r');
        *end = '\0';
        content_length = (size_t) atoi(temp);
        *end = '\r';
        return content_length;
    }

    *temp = ':';
    return 0;
}


// ----------- PROXY UTILITY -----------

char *find_index(char *string, char token) {
    char *loc;
    if ((loc = strchr(string, token)) == NULL)
        client_error("Invalid HTTP Header");
    return loc;
}

void client_error(char *msg) {
    fprintf(stderr, "%s\n", msg);
    exit(1);
}



/* ============================================
                CACHE MANAGEMENT
   ============================================ */


// ----------- CACHE USER LEVEL FUNCTIONS -----------

http_object *cache_fetch(char *hostname, char *port, char *uri) {
    cache_list *used;

    // Look through cache
    if ((used = cache_lookup(hostname, port, uri)) == NULL)
        return NULL;

    // Update linked list
    cache_update(used);

    return used->cache_object;
}

void cache_push(http_object_identifier *object_identifier, size_t content_length, char *content) {
    cache_list *new = create_cache_element(object_identifier, content_length, content);

    // Evict until enough space is avaliable
    while (content_length + cache.size > MAX_CACHE_SIZE) {
        cache_evict();
    }

    // Update linked list
    cache_update(new);
    return;
}

void assemble_response(int client_fd, http_object *cached_object) {
    // Rio_writen(STDOUT_FILENO, cached_object->content, cached_object->content_length); 
    Rio_writen(client_fd, cached_object->content, cached_object->content_length);
    return;
}

// ----------- CACHE INTERNAL LOGIC -----------

cache_list *cache_lookup(char *hostname, char *port, char *uri) {
    cache_list *cp;
    http_object_identifier *object_identifier;

    for (cp = cache.mru; cp != NULL; cp = cp->prev) {
        object_identifier = cp->cache_object->object_identifier;
        
        // Compare
        if (
            (strcmp(object_identifier->hostname, hostname) == 0) 
            && 
            (strcmp(object_identifier->port, port) == 0)
            && 
            (strcmp(object_identifier->uri, uri) == 0)
        )
            return cp;
    }

    return NULL;
}

void cache_update(cache_list *used) {
    cache_list *prev, *next;

    // Case 1: Empty Cache
    if (cache.size == 0) {
        cache.mru = used;
        cache.lru = used;
        cache.size = used->cache_object->content_length;
        return;
    }
    // Case 2: Is the MRU
    if (cache.mru == used)
        return;

    prev = used->prev;
    next = used->next;
    // Case 3: Is the LRU
    if (cache.lru == used) {
        cache.lru = used->next;
        next->prev = NULL;
    }
    // Case 4: Detachted (Not added yet)
    else if ((next == NULL) && (prev == NULL)) {
        cache.size += used->cache_object->content_length;

    }
    // Case 5: In the middle
    else {
        prev->next = next;
        next->prev = prev;
    }

    used->prev = cache.mru;
    used->next = NULL;
    cache.mru->next = used;
    cache.mru = used;
    return;
}

void cache_evict() {
    cache_list *lru = cache.lru;

    // Update LRU
    cache.lru = lru->next;
    cache.lru->prev = NULL;

    // Free cache element
    free_cache_element(lru);
    return;
}



// ----------- CACHE UTILITY -----------

http_object_identifier *create_http_object_identifier(char *hostname, char *port, char *uri) {
    http_object_identifier *object_identifier = (http_object_identifier *) Malloc(sizeof(http_object_identifier));
    
    object_identifier->hostname = (char *) Malloc(sizeof(char) * (strlen(hostname) + 1));
    object_identifier->port = (char *) Malloc(sizeof(char) * (strlen(port) + 1));
    object_identifier->uri = (char *) Malloc(sizeof(char) * (strlen(uri) + 1));

    strcpy(object_identifier->hostname, hostname);
    strcpy(object_identifier->port, port);
    strcpy(object_identifier->uri, uri);

    return object_identifier;
}

cache_list *create_cache_element(http_object_identifier *object_identifier, size_t content_length, char *content) {
    http_object *cache_object = (http_object *) Malloc(sizeof(http_object));
    cache_list *new = (cache_list *) Malloc(sizeof(cache_list));

    cache_object->object_identifier = object_identifier;
    cache_object->content_length = content_length;
    cache_object->content = content;

    new->cache_object = cache_object;
    new->prev = NULL;
    new->next = NULL;

    return new;
}

void free_cache_element(cache_list *cache_element) {
    http_object *cache_object = cache_element->cache_object;
    http_object_identifier *object_identifier = cache_object->object_identifier;

    // Free object identifier
    Free(object_identifier->hostname);
    Free(object_identifier->port);
    Free(object_identifier->uri);
    Free(object_identifier);

    // Free http object
    Free(cache_object->content);
    Free(cache_object);

    // Free cache element
    Free(cache_element);
}