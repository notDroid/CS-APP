## Solutions With Notes
### Phase 1:
- Call <strings_not_equal>
- Compare string length (<string_length>), correct length: 52
- The argument for this called is initially in %rbp, examining %rbp gives:

**Answer**: "Border relations with Canada have never been better."

### Phase 2:
- Call <read_six_numbers>, then call <__isoc99_sscanf@plt>
- If %eax <= 5 explode.
