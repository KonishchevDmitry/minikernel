### References

* BIOS interrupts - http://www.ctyme.com/intr/int.htm
* x86 instructions - https://www.felixcloutier.com/x86/
* GNU as manual - https://sourceware.org/binutils/docs-2.40/as.html

### Registers

#### General-purpose

* EAX – Accumulator for operands and results data
* EBX – Pointer to data in the data memory segment
* ECX – Counter for string and loop operations
* EDX – I/O pointer
* EDI – Data pointer for destination of string operations
* ESI – Data pointer for source of string operations
* ESP – Stack pointer
* EBP – Stack data pointer

#### Segment (all are 16 bits)

* CS – Code segment
* DS – Data segment
* SS – Stack segment
* ES – Extra segment pointer
* FS – Extra segment pointer
* GS – Extra segment pointer


### C calling convention

EAX, ECX, EDX may be modified by callee.


### Indexing

`base(offset, index, size)` -> `base + offset + index * size`

If any of the values are zero, they can be omitted (but the commas are still required as placeholders). The `offset` and `index` must be registers, but the `size` can be a numerical value.


### GDB

```
break *label+line_offset
run
next
info registers
print/{d,h,t} $eax
cont
```

Print memory – `x/nyz &label`:
* `n` – count
* `y` – c/d/x
* `z` – b (byte), h (2 bytes), w (4 bytes)