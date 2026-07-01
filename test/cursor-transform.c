/*
 * Copyright © 2026 XLibre
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 */

/* Test relies on assert() */
#undef NDEBUG

#include <dix-config.h>

#include <assert.h>
#include <stddef.h>

#include "tests.h"
#include "xf86CursorTransform.h"

static void
cursor_transform_size(void)
{
    int width, height;

    xf86CursorTransformedSize(RR_Rotate_0, 10, 7, &width, &height);
    assert(width == 10);
    assert(height == 7);

    xf86CursorTransformedSize(RR_Rotate_90, 10, 7, &width, &height);
    assert(width == 7);
    assert(height == 10);

    xf86CursorTransformedSize(RR_Rotate_180, 10, 7, &width, &height);
    assert(width == 10);
    assert(height == 7);

    xf86CursorTransformedSize(RR_Rotate_270, 10, 7, &width, &height);
    assert(width == 7);
    assert(height == 10);

    xf86CursorTransformedSize(RR_Rotate_0 | RR_Reflect_X, 10, 7,
                              &width, &height);
    assert(width == 10);
    assert(height == 7);

    xf86CursorTransformedSize(RR_Rotate_0 | RR_Reflect_Y, 10, 7,
                              &width, &height);
    assert(width == 10);
    assert(height == 7);

    xf86CursorTransformedSize(RR_Rotate_90 | RR_Reflect_X, 10, 7,
                              &width, &height);
    assert(width == 7);
    assert(height == 10);

    xf86CursorTransformedSize(RR_Rotate_270 | RR_Reflect_X, 10, 7,
                              &width, &height);
    assert(width == 7);
    assert(height == 10);

    xf86CursorTransformedSize(0, 10, 7, &width, &height);
    assert(width == 10);
    assert(height == 7);
}

static void
cursor_rotate_coord_roundtrip(void)
{
    const Rotation rotations[] = {
        RR_Rotate_0,
        RR_Rotate_90,
        RR_Rotate_180,
        RR_Rotate_270,
        RR_Rotate_0 | RR_Reflect_X,
        RR_Rotate_0 | RR_Reflect_Y,
        RR_Rotate_90 | RR_Reflect_X,
        RR_Rotate_270 | RR_Reflect_Y,
    };

    for (unsigned int r = 0; r < sizeof(rotations) / sizeof(rotations[0]); r++) {
        int width = 5;
        int height = 3;
        int transformed_width, transformed_height;

        xf86CursorTransformedSize(rotations[r], width, height,
                                  &transformed_width, &transformed_height);

        for (int y = 0; y < transformed_height; y++) {
            for (int x = 0; x < transformed_width; x++) {
                int src_x, src_y;
                int dst_x, dst_y;

                xf86CursorRotateCoord(rotations[r], width, height,
                                      x, y, &src_x, &src_y);
                xf86CursorRotateCoordBack(rotations[r], width, height,
                                          src_x, src_y, &dst_x, &dst_y);
                assert(dst_x == x);
                assert(dst_y == y);
            }
        }
    }
}

static void
cursor_default_rotation_identity(void)
{
    int src_x, src_y;
    int dst_x, dst_y;
    int width, height;
    xf86CursorPositionTransformRec t;
    int hw_x, hw_y;

    xf86CursorRotateCoord(0, 9, 6, 4, 2, &src_x, &src_y);
    assert(src_x == 4);
    assert(src_y == 2);

    xf86CursorRotateCoordBack(0, 9, 6, 4, 2, &dst_x, &dst_y);
    assert(dst_x == 4);
    assert(dst_y == 2);

    xf86CursorTransformedSize(0, 9, 6, &width, &height);
    assert(width == 9);
    assert(height == 6);

    xf86CursorRotateCoord(RR_Rotate_180 | RR_Reflect_X | RR_Reflect_Y,
                          9, 6, 4, 2, &src_x, &src_y);
    assert(src_x == 4);
    assert(src_y == 2);

    xf86CursorRotateCoordBack(RR_Rotate_180 | RR_Reflect_X | RR_Reflect_Y,
                              9, 6, 4, 2, &dst_x, &dst_y);
    assert(dst_x == 4);
    assert(dst_y == 2);

    xf86CursorComputePositionTransform(RR_Rotate_180 |
                                       RR_Reflect_X | RR_Reflect_Y,
                                       100, 50, 640, 480, 0, 0, 0, 0, &t);
    assert(t.op == XF86_CURSOR_POS_X_Y);
    xf86CursorApplyPositionTransform(&t, 110, 70, &hw_x, &hw_y);
    assert(hw_x == 10);
    assert(hw_y == 20);
}

static void
cursor_position_transform(void)
{
    xf86CursorPositionTransformRec t;
    int cursor_hot_x, cursor_hot_y;
    int hw_x, hw_y;

    xf86CursorComputePositionTransform(RR_Rotate_90, 100, 50, 640, 480,
                                       3, 4, 5, 6, &t);
    assert(t.op == XF86_CURSOR_POS_Y_NEG_X);

    xf86CursorRotateCoordBack(RR_Rotate_270, 9, 6, 2, 1,
                              &cursor_hot_x, &cursor_hot_y);
    assert(cursor_hot_x == 4);
    assert(cursor_hot_y == 2);

    xf86CursorComputePositionTransform(RR_Rotate_270, 100, 50, 640, 480,
                                       2, 1, cursor_hot_x, cursor_hot_y, &t);
    assert(t.op == XF86_CURSOR_POS_NEG_Y_X);
    xf86CursorApplyPositionTransform(&t, 110, 70, &hw_x, &hw_y);
    assert(hw_x == 614);
    assert(hw_y == 10);
}

static void
cursor_position_apply_ops(void)
{
    xf86CursorPositionTransformRec t;
    int hw_x, hw_y;

    t.x_offset = 1000;
    t.y_offset = 2000;

    t.op = XF86_CURSOR_POS_X_Y;
    xf86CursorApplyPositionTransform(&t, 11, 17, &hw_x, &hw_y);
    assert(hw_x == 1011);
    assert(hw_y == 2017);

    t.op = XF86_CURSOR_POS_X_NEG_Y;
    xf86CursorApplyPositionTransform(&t, 11, 17, &hw_x, &hw_y);
    assert(hw_x == 1011);
    assert(hw_y == 1983);

    t.op = XF86_CURSOR_POS_NEG_X_Y;
    xf86CursorApplyPositionTransform(&t, 11, 17, &hw_x, &hw_y);
    assert(hw_x == 989);
    assert(hw_y == 2017);

    t.op = XF86_CURSOR_POS_NEG_X_NEG_Y;
    xf86CursorApplyPositionTransform(&t, 11, 17, &hw_x, &hw_y);
    assert(hw_x == 989);
    assert(hw_y == 1983);

    t.op = XF86_CURSOR_POS_Y_X;
    xf86CursorApplyPositionTransform(&t, 11, 17, &hw_x, &hw_y);
    assert(hw_x == 1017);
    assert(hw_y == 2011);

    t.op = XF86_CURSOR_POS_Y_NEG_X;
    xf86CursorApplyPositionTransform(&t, 11, 17, &hw_x, &hw_y);
    assert(hw_x == 1017);
    assert(hw_y == 1989);

    t.op = XF86_CURSOR_POS_NEG_Y_X;
    xf86CursorApplyPositionTransform(&t, 11, 17, &hw_x, &hw_y);
    assert(hw_x == 983);
    assert(hw_y == 2011);

    t.op = XF86_CURSOR_POS_NEG_Y_NEG_X;
    xf86CursorApplyPositionTransform(&t, 11, 17, &hw_x, &hw_y);
    assert(hw_x == 983);
    assert(hw_y == 1989);
}

static void
cursor_position_compute_matches_point_transform(void)
{
    const Rotation rotations[] = {
        RR_Rotate_0,
        RR_Rotate_90,
        RR_Rotate_180,
        RR_Rotate_270,
        RR_Rotate_0 | RR_Reflect_X,
        RR_Rotate_0 | RR_Reflect_Y,
        RR_Rotate_90 | RR_Reflect_X,
        RR_Rotate_90 | RR_Reflect_Y,
        RR_Rotate_180 | RR_Reflect_X,
        RR_Rotate_180 | RR_Reflect_X | RR_Reflect_Y,
        RR_Rotate_270 | RR_Reflect_Y,
        RR_Rotate_90 | RR_Reflect_X | RR_Reflect_Y,
    };

    for (unsigned int r = 0; r < sizeof(rotations) / sizeof(rotations[0]); r++) {
        xf86CursorPositionTransformRec t;
        int expected_x = 110 + 3;
        int expected_y = 70 + 4;
        int hw_x, hw_y;

        xf86CursorComputePositionTransform(rotations[r], 100, 50, 640, 480,
                                           3, 4, 5, 6, &t);
        xf86CursorTransformPoint(rotations[r], 100, 50, 640, 480,
                                 &expected_x, &expected_y);
        expected_x -= 5;
        expected_y -= 6;

        xf86CursorApplyPositionTransform(&t, 110, 70, &hw_x, &hw_y);
        assert(hw_x == expected_x);
        assert(hw_y == expected_y);
    }
}

static void
cursor_screen_range(void)
{
    assert(xf86CursorInScreenRange(100, 50, 480, 640, 64, 64, 100, 50));
    assert(xf86CursorInScreenRange(100, 50, 480, 640, 64, 64, 579, 689));
    assert(xf86CursorInScreenRange(100, 50, 480, 640, 64, 64, 37, -13));
    assert(!xf86CursorInScreenRange(100, 50, 480, 640, 64, 64, 580, 50));
    assert(!xf86CursorInScreenRange(100, 50, 480, 640, 64, 64, 100, 690));
    assert(!xf86CursorInScreenRange(100, 50, 480, 640, 64, 64, 36, 50));
    assert(!xf86CursorInScreenRange(100, 50, 480, 640, 64, 64, 100, -14));
}

static void
cursor_rectangular_screen_range(void)
{
    int cursor_width, cursor_height;

    xf86CursorTransformedSize(RR_Rotate_90, 10, 7,
                              &cursor_width, &cursor_height);
    assert(cursor_width == 7);
    assert(cursor_height == 10);

    assert(xf86CursorInScreenRange(100, 50, 480, 640,
                                   cursor_width, cursor_height, 94, 50));
    assert(!xf86CursorInScreenRange(100, 50, 480, 640,
                                    cursor_width, cursor_height, 93, 50));
    assert(xf86CursorInScreenRange(100, 50, 480, 640,
                                   cursor_width, cursor_height, 100, 41));
    assert(!xf86CursorInScreenRange(100, 50, 480, 640,
                                    cursor_width, cursor_height, 100, 40));

    xf86CursorTransformedSize(RR_Rotate_0, 10, 7,
                              &cursor_width, &cursor_height);
    assert(cursor_width == 10);
    assert(cursor_height == 7);

    assert(xf86CursorInScreenRange(100, 50, 480, 640,
                                   cursor_width, cursor_height, 91, 50));
    assert(!xf86CursorInScreenRange(100, 50, 480, 640,
                                    cursor_width, cursor_height, 90, 50));
    assert(xf86CursorInScreenRange(100, 50, 480, 640,
                                   cursor_width, cursor_height, 100, 44));
    assert(!xf86CursorInScreenRange(100, 50, 480, 640,
                                    cursor_width, cursor_height, 100, 43));
}

const testfunc_t *
cursor_transform_test(void)
{
    static const testfunc_t tests[] = {
        cursor_transform_size,
        cursor_rotate_coord_roundtrip,
        cursor_default_rotation_identity,
        cursor_position_transform,
        cursor_position_apply_ops,
        cursor_position_compute_matches_point_transform,
        cursor_screen_range,
        cursor_rectangular_screen_range,
        NULL,
    };

    return tests;
}
