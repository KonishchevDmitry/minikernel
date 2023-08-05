// Provides macros for writting portable assembly code

#if ARCH_BITS == 16
    .code16
    #define ARCH_REGISTER(base) base
#elif ARCH_BITS == 32
    .code32
    #define ARCH_REGISTER(base) e ## base
#else
    #error "Invalid architecture configuration"
#endif

#define S(count) (ARCH_BITS / 8 * count)

#define AX ARCH_REGISTER(ax)
#define BX ARCH_REGISTER(bx)
#define CX ARCH_REGISTER(cx)
#define DX ARCH_REGISTER(dx)
#define BP ARCH_REGISTER(bp)
#define SP ARCH_REGISTER(sp)
#define SI ARCH_REGISTER(si)
#define DI ARCH_REGISTER(di)