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
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 */

#include "xf86CursorTransform.h"

void
xf86CursorRotateCoord(Rotation rotation, int width, int height,
                      int x_dst, int y_dst, int *x_src, int *y_src)
{
    int t;

    switch (rotation & 0xf) {
    case RR_Rotate_90:
        t = x_dst;
        x_dst = width - y_dst - 1;
        y_dst = t;
        break;
    case RR_Rotate_180:
        x_dst = width - x_dst - 1;
        y_dst = height - y_dst - 1;
        break;
    case RR_Rotate_270:
        t = x_dst;
        x_dst = y_dst;
        y_dst = height - t - 1;
        break;
    case RR_Rotate_0:
    default:
        break;
    }

    if (rotation & RR_Reflect_X)
        x_dst = width - x_dst - 1;
    if (rotation & RR_Reflect_Y)
        y_dst = height - y_dst - 1;

    *x_src = x_dst;
    *y_src = y_dst;
}

void
xf86CursorRotateCoordBack(Rotation rotation, int width, int height,
                          int x_src, int y_src, int *x_dst, int *y_dst)
{
    int t;

    if (rotation & RR_Reflect_X)
        x_src = width - x_src - 1;
    if (rotation & RR_Reflect_Y)
        y_src = height - y_src - 1;

    switch (rotation & 0xf) {
    case RR_Rotate_90:
        t = x_src;
        x_src = y_src;
        y_src = width - t - 1;
        break;
    case RR_Rotate_180:
        x_src = width - x_src - 1;
        y_src = height - y_src - 1;
        break;
    case RR_Rotate_270:
        t = x_src;
        x_src = height - y_src - 1;
        y_src = t;
        break;
    case RR_Rotate_0:
    default:
        break;
    }

    *x_dst = x_src;
    *y_dst = y_src;
}

void
xf86CursorTransformedSize(Rotation rotation, int width, int height,
                          int *transformed_width,
                          int *transformed_height)
{
    switch (rotation & 0xf) {
    case RR_Rotate_90:
    case RR_Rotate_270:
        *transformed_width = height;
        *transformed_height = width;
        break;
    case RR_Rotate_0:
    case RR_Rotate_180:
    default:
        *transformed_width = width;
        *transformed_height = height;
        break;
    }
}

void
xf86CursorTransformPoint(Rotation rotation, int crtc_x, int crtc_y,
                         int mode_width, int mode_height,
                         int *x, int *y)
{
    int t;
    int swap_reflection = 0;

    *x -= crtc_x;
    *y -= crtc_y;

    switch (rotation & 0xf) {
    case RR_Rotate_90:
        t = *x;
        *x = *y;
        *y = mode_height - t - 1;
        swap_reflection = 1;
        break;
    case RR_Rotate_180:
        *x = mode_width - *x - 1;
        *y = mode_height - *y - 1;
        break;
    case RR_Rotate_270:
        t = *x;
        *x = mode_width - *y - 1;
        *y = t;
        swap_reflection = 1;
        break;
    case RR_Rotate_0:
    default:
        break;
    }

    if (swap_reflection) {
        if (rotation & RR_Reflect_Y)
            *x = mode_width - *x - 1;
        if (rotation & RR_Reflect_X)
            *y = mode_height - *y - 1;
    } else {
        if (rotation & RR_Reflect_X)
            *x = mode_width - *x - 1;
        if (rotation & RR_Reflect_Y)
            *y = mode_height - *y - 1;
    }
}

void
xf86CursorComputePositionTransform(Rotation rotation,
                                   int crtc_x, int crtc_y,
                                   int mode_width, int mode_height,
                                   int source_hot_x, int source_hot_y,
                                   int cursor_hot_x, int cursor_hot_y,
                                   xf86CursorPositionTransformRec *t)
{
    int x0 = source_hot_x, y0 = source_hot_y;
    int x1 = source_hot_x + 1, y1 = source_hot_y;
    int x2 = source_hot_x, y2 = source_hot_y + 1;
    int ax, ay, cx, cy;

    xf86CursorTransformPoint(rotation, crtc_x, crtc_y, mode_width, mode_height,
                             &x0, &y0);
    xf86CursorTransformPoint(rotation, crtc_x, crtc_y, mode_width, mode_height,
                             &x1, &y1);
    xf86CursorTransformPoint(rotation, crtc_x, crtc_y, mode_width, mode_height,
                             &x2, &y2);

    ax = x1 - x0;
    ay = x2 - x0;
    cx = y1 - y0;
    cy = y2 - y0;

    t->op = XF86_CURSOR_POS_X_Y;
    if (ax == 1 && ay == 0 && cx == 0 && cy == -1)
        t->op = XF86_CURSOR_POS_X_NEG_Y;
    else if (ax == -1 && ay == 0 && cx == 0 && cy == 1)
        t->op = XF86_CURSOR_POS_NEG_X_Y;
    else if (ax == -1 && ay == 0 && cx == 0 && cy == -1)
        t->op = XF86_CURSOR_POS_NEG_X_NEG_Y;
    else if (ax == 0 && ay == 1 && cx == 1 && cy == 0)
        t->op = XF86_CURSOR_POS_Y_X;
    else if (ax == 0 && ay == 1 && cx == -1 && cy == 0)
        t->op = XF86_CURSOR_POS_Y_NEG_X;
    else if (ax == 0 && ay == -1 && cx == 1 && cy == 0)
        t->op = XF86_CURSOR_POS_NEG_Y_X;
    else if (ax == 0 && ay == -1 && cx == -1 && cy == 0)
        t->op = XF86_CURSOR_POS_NEG_Y_NEG_X;

    /*
     * The legacy KMS cursor move ioctl always takes the cursor BO's upper-left
     * position.  drmModeSetCursor2() accepts hotspot metadata, but that
     * metadata does not change drmModeMoveCursor() coordinates.
     */
    t->x_offset = x0 - cursor_hot_x;
    t->y_offset = y0 - cursor_hot_y;
}

void
xf86CursorApplyPositionTransform(const xf86CursorPositionTransformRec *t,
                                 int x, int y, int *hw_x, int *hw_y)
{
    switch (t->op) {
    case XF86_CURSOR_POS_X_Y:
        *hw_x = x + t->x_offset;
        *hw_y = y + t->y_offset;
        break;
    case XF86_CURSOR_POS_X_NEG_Y:
        *hw_x = x + t->x_offset;
        *hw_y = t->y_offset - y;
        break;
    case XF86_CURSOR_POS_NEG_X_Y:
        *hw_x = t->x_offset - x;
        *hw_y = y + t->y_offset;
        break;
    case XF86_CURSOR_POS_NEG_X_NEG_Y:
        *hw_x = t->x_offset - x;
        *hw_y = t->y_offset - y;
        break;
    case XF86_CURSOR_POS_Y_X:
        *hw_x = y + t->x_offset;
        *hw_y = x + t->y_offset;
        break;
    case XF86_CURSOR_POS_Y_NEG_X:
        *hw_x = y + t->x_offset;
        *hw_y = t->y_offset - x;
        break;
    case XF86_CURSOR_POS_NEG_Y_X:
        *hw_x = t->x_offset - y;
        *hw_y = x + t->y_offset;
        break;
    case XF86_CURSOR_POS_NEG_Y_NEG_X:
        *hw_x = t->x_offset - y;
        *hw_y = t->y_offset - x;
        break;
    default:
        *hw_x = x + t->x_offset;
        *hw_y = y + t->y_offset;
        break;
    }
}

Bool
xf86CursorInScreenRange(int crtc_x, int crtc_y,
                        int crtc_width, int crtc_height,
                        int cursor_width, int cursor_height,
                        int x, int y)
{
    return x < crtc_x + crtc_width &&
           y < crtc_y + crtc_height &&
           x > crtc_x - cursor_width &&
           y > crtc_y - cursor_height;
}
