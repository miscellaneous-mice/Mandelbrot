## Comparison Operations
| Operation             | NEON Intrinsic   | Scalar Equivalent |
| --------------------- | ---------------- | ----------------- |
| Equal                 | `vceqq_f64(a,b)` | `a==b`            |
| Less than             | `vcltq_f64(a,b)` | `a<b`             |
| Less than or equal    | `vcleq_f64(a,b)` | `a<=b`            |
| Greater than          | `vcgtq_f64(a,b)` | `a>b`             |
| Greater than or equal | `vcgeq_f64(a,b)` | `a>=b`            |

## Arthematic Operations
| Operation          | NEON Intrinsic     | Equivalent Scalar Code     |
| ------------------ | ------------------ | -------------------------- |
| Add                | `vaddq_f64(a,b)`   | `a+b`                      |
| Subtract           | `vsubq_f64(a,b)`   | `a-b`                      |
| Multiply           | `vmulq_f64(a,b)`   | `a*b`                      |
| Divide             | `vdivq_f64(a,b)`   | `a/b`                      |
| Square             | `vmulq_f64(a,a)`   | `a*a`                      |
| Negate             | `vnegq_f64(a)`     | `-a`                       |
| Absolute value     | `vabsq_f64(a)`     | `abs(a)`                   |
| Duplicate scalar   | `vdupq_n_f64(x)`   | `[x,x]`                    |
| Load from array    | `vld1q_f64(ptr)`   | `{ptr[0],ptr[1]}`          |
| Store to array     | `vst1q_f64(ptr,a)` | `ptr[0]=a[0], ptr[1]=a[1]` |
| Fused multiply-add | `vfmaq_f64(c,a,b)` | `c+a*b`                    |
| Min                | `vminq_f64(a,b)`   | `min(a,b)`                 |
| Max                | `vmaxq_f64(a,b)`   | `max(a,b)`                 |
| Square root        | `vsqrtq_f64(a)`    | `sqrt(a)`                  |

## Bitwise Operations (uint64x2_t)

| Operation | Intrinsic        | Scalar Equivalent |
| --------- | ---------------- | ----------------- |
| AND       | `vandq_u64(a,b)` | `a & b`           |
| OR        | `vorrq_u64(a,b)` | `a \| b`          |
| XOR       | `veorq_u64(a,b)` | `a ^ b`           |
| NOT       | `vmvnq_u64(a)`   | `~a`              |
