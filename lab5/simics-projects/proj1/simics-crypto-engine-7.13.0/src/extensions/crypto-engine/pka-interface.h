/*
  © 2010 Intel Corporation

  This software and the related documents are Intel copyrighted materials, and
  your use of them is governed by the express license under which they were
  provided to you ("License"). Unless the License provides otherwise, you may
  not use, modify, copy, publish, distribute, disclose or transmit this software
  or the related documents without Intel's prior written permission.

  This software and the related documents are provided as is, with no express or
  implied warranties, other than those that are expressly stated in the License.
*/

#ifndef __PKA_INTERFACE_H__
#define __PKA_INTERFACE_H__

#include <simics/base/types.h>
#include <simics/base/conf-object.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* Errors */
typedef enum {
        Pka_Err_None = 0,
        Pka_Err_Modular_Err,
        Pka_Err_Unknown_Field,
        Pka_Err_Unknown_Coord,
        Pka_Err_Prime_Test,
        Pka_Err_Ec_Mul_Scalar,
        Pka_Err_Para,
        Pka_Err_Data_Fmt,
        Pka_Err_Data_Size,
        Pka_Err_Point_A_Not_On_Curve,
        Pka_Err_Point_B_Not_On_Curve,
        Pka_Err_Div_By_Zero,
        Pka_Err_Mod_Even,
        Pka_Err_General,
} pka_error_t;

/* BN functions */
typedef enum {
        Pka_Bn_Mod_Add = 0,
        Pka_Bn_Mod_Sub,
        Pka_Bn_Mod_Mul,
        Pka_Bn_Mod_Exp,
        Pka_Bn_Mod_Reduction,
        Pka_Bn_Mod_Inversion,
        Pka_Bn_Gcd,
        Pka_Bn_Prime_Test,
        Pka_Bn_Gen_Prime,
        Pka_Bn_R2,
        Pka_Bn_ReRp,
        Pka_Bn_Add,
        Pka_Bn_Sub,
        Pka_Bn_Mul,
} pka_bn_func_t;

/* Field types */
typedef enum {
        Pka_Field_Fp = 0,
        Pka_Field_F2m,
} pka_field_t;

/* Coordinate systems */
typedef enum {
        Pka_Coord_Affine = 0,
        Pka_Coord_Projective,
} pka_coord_t;

/* Input/output format */
typedef enum {
        Pka_Data_Fmt_Normal = 0,
        Pka_Data_Fmt_Montgomery,
} pka_data_fmt_t;

typedef struct {
    void (*to_montgomery)(
        conf_object_t *obj, const bytes_t m, const bytes_t *x,
        buffer_t **r, size_t n);
    void (*from_montgomery)(
        conf_object_t *obj, const bytes_t m, const bytes_t *x,
        buffer_t **r, size_t n);

    pka_error_t
        (*bn)(conf_object_t *obj, pka_bn_func_t func, pka_field_t field,
              pka_data_fmt_t in_fmt, pka_data_fmt_t out_fmt,
              const bytes_t a, const bytes_t b, const bytes_t m,
              buffer_t *r);

    pka_error_t (*ec_mod_add)(
        conf_object_t *obj, pka_field_t field,
        pka_data_fmt_t in_fmt, pka_data_fmt_t out_fmt, pka_coord_t coord,
        const bytes_t curve_a, const bytes_t curve_b, const bytes_t m,
        const bytes_t p0[3], const bytes_t p1[3], buffer_t *r[3]);

    pka_error_t (*ec_mod_dbl)(
        conf_object_t *obj, pka_field_t field,
        pka_data_fmt_t in_fmt, pka_data_fmt_t out_fmt, pka_coord_t coord,
        const bytes_t curve_a, const bytes_t curve_b, const bytes_t m,
        const bytes_t p0[3], buffer_t *r[3]);

    pka_error_t (*ec_mod_mul)(
        conf_object_t *obj, pka_field_t field,
        pka_data_fmt_t in_fmt, pka_data_fmt_t out_fmt, pka_coord_t coord,
        const bytes_t curve_a, const bytes_t curve_b, const bytes_t m,
        const bytes_t s, const bytes_t p0[3], buffer_t *r[3]);

    void (*montgomery_mul)(
            conf_object_t *obj, const bytes_t *a, const bytes_t *b,
            const bytes_t *m, buffer_t *r);

    pka_error_t (*bn_div)(
        conf_object_t *obj,
        const bytes_t a, const bytes_t b, buffer_t *q, buffer_t *r);
} pka_interface_t;
#define PKA_INTERFACE "pka"

#if defined(__cplusplus)
}
#endif

#endif  /* __PKA_INTERFACE_H__ */
