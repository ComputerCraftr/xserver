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

#ifndef XF86_CURSOR_TRANSFORM_H
#define XF86_CURSOR_TRANSFORM_H

#include <X11/extensions/randr.h>
#include <X11/Xdefs.h>

typedef enum {
    XF86_CURSOR_POS_X_Y,
    XF86_CURSOR_POS_X_NEG_Y,
    XF86_CURSOR_POS_NEG_X_Y,
    XF86_CURSOR_POS_NEG_X_NEG_Y,
    XF86_CURSOR_POS_Y_X,
    XF86_CURSOR_POS_Y_NEG_X,
    XF86_CURSOR_POS_NEG_Y_X,
    XF86_CURSOR_POS_NEG_Y_NEG_X,
} xf86CursorPositionOp;

typedef struct _xf86CursorPositionTransform {
    xf86CursorPositionOp op;
    int x_offset;
    int y_offset;
} xf86CursorPositionTransformRec;

void xf86CursorRotateCoord(Rotation rotation, int width, int height,
                           int x_dst, int y_dst, int *x_src, int *y_src);
void xf86CursorRotateCoordBack(Rotation rotation, int width, int height,
                               int x_src, int y_src, int *x_dst, int *y_dst);
void xf86CursorTransformedSize(Rotation rotation, int width, int height,
                               int *transformed_width,
                               int *transformed_height);
void xf86CursorTransformPoint(Rotation rotation, int crtc_x, int crtc_y,
                              int mode_width, int mode_height,
                              int *x, int *y);
void xf86CursorComputePositionTransform(Rotation rotation,
                                        int crtc_x, int crtc_y,
                                        int mode_width, int mode_height,
                                        int source_hot_x, int source_hot_y,
                                        int cursor_hot_x, int cursor_hot_y,
                                        xf86CursorPositionTransformRec *t);
void xf86CursorApplyPositionTransform(const xf86CursorPositionTransformRec *t,
                                      int x, int y, int *hw_x, int *hw_y);
Bool xf86CursorInScreenRange(int crtc_x, int crtc_y,
                             int crtc_width, int crtc_height,
                             int cursor_width, int cursor_height,
                             int x, int y);

#endif
