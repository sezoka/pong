#include <raylib.h>
#include <stdio.h>

#define PLATFORM_WIDTH 10
#define PLATFORM_SPEED 200

typedef struct {
    float x;
    float y;
    float vx;
    float vy;
} Ball;

typedef struct {
    float y;
    float h;
} Platform;

typedef struct {
    int field_w;
    int field_h;
    Ball ball;
    Platform left_platform;
    Platform right_platform;
} State;

void init_ball(Ball* b) {
    *b = (Ball){0};
}

void init_platform(Platform* p) {
    p->y = 10;
    p->h = 40;
}

void init_state(State* s) {
    init_ball(&s->ball);
    init_platform(&s->left_platform);
    init_platform(&s->right_platform);
    s->field_w = 200;
    s->field_h = 200;
}

int is_ball_collide(State* s) {
    Ball* b = &s->ball;
    Platform* lp = &s->left_platform;
    Platform* rp = &s->right_platform;

    Rectangle r0 = {b->x, b->y, 10, 10};
    Rectangle r1 = {10, lp->y, 10, lp->h};
    Rectangle r2 = {GetScreenWidth() - 20, rp->y, 10, rp->h};

    return CheckCollisionRecs(r0, r1) || CheckCollisionRecs(r0, r2);
}

void update_ball(State* s) {
    Ball ball = s->ball;
    int ball_w = 10;

    float field_w = GetScreenWidth();
    float field_h = GetScreenHeight();

    float delta = GetFrameTime();

    ball.x += ball.vx * delta;
    if (is_ball_collide(s)) {
        ball.x = s->ball.x;
        ball.vx = -ball.vx;
    } else if (ball.x < 0) {
        ball.x = 0;
        ball.vx = -ball.vx;
    } else if (field_w - ball_w < ball.x) {
        ball.x = field_w - ball_w;
        ball.vx = -ball.vx;
    }

    ball.y += ball.vy * delta;
    if (is_ball_collide(s)) {
        ball.y = s->ball.y;
        ball.vy = -ball.vy;
    } else if (ball.y < 0) {
        ball.y = 0;
        ball.vy = -ball.vy;
    } else if (field_h - ball_w < ball.y) {
        ball.y = field_h - ball_w;
        ball.vy = -ball.vy;
    }

    s->ball = ball;
}

void update_panel(State* s, Platform* p, int up, int down) {
    float field_w = GetScreenWidth();
    float field_h = GetScreenHeight();

    float delta = GetFrameTime();

    if (IsKeyDown(up))
        p->y -= PLATFORM_SPEED * delta;
    if (IsKeyDown(down))
        p->y += PLATFORM_SPEED * delta;

    if (p->y < 0) {
        p->y = 0;
    } else if (field_h - p->h < p->y) {
        p->y = field_h - p->h;
    }
}

void update_panels(State* s) {
    update_panel(s, &s->left_platform, KEY_W, KEY_S);
    update_panel(s, &s->right_platform, KEY_UP, KEY_DOWN);
}

void update_state(State* s) {
    update_panels(s);
    update_ball(s);
}

int main() {
    InitWindow(800, 400, "pong");
    SetWindowState(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);

    State state;
    init_state(&state);

    state.ball.vx = 200;
    state.ball.vy = 200;

    char fps_string[10];

    while (!WindowShouldClose()) {
        update_state(&state);

        BeginDrawing();
        {
            ClearBackground(RAYWHITE);
            DrawRectangle(state.ball.x, state.ball.y, 10, 10, BLACK);
            DrawRectangle(10, state.left_platform.y, 10, state.left_platform.h,
                          BLACK);
            DrawRectangle(GetScreenWidth() - 20, state.right_platform.y, 10,
                          state.right_platform.h, BLACK);
            sprintf(fps_string, "%d", GetFPS());
            DrawText(fps_string, 0, 0, 16, BLACK);
        }
        EndDrawing();
    }

    return 0;
}
