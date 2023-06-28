#include <raylib.h>
#include <stdio.h>

#define PADDLE_WIDTH 10
#define PADDLE_HEIGHT 50
#define PADDLE_SPEED 200
#define BALL_SIZE 10

typedef struct {
    float x;
    float y;
    float vx;
    float vy;
} Ball;

typedef struct {
    float x;
    float y;
    float h;
    float w;
} Paddle;

typedef struct {
    int field_w;
    int field_h;
    Ball ball;
    Paddle left_paddle;
    Paddle right_paddle;
    int left_player_score;
    int right_player_score;
} State;

void init_state(State* s) {
    float scr_w = GetScreenWidth();
    float scr_h = GetScreenHeight();

    s->left_paddle.x = PADDLE_WIDTH;
    s->left_paddle.h = PADDLE_HEIGHT;
    s->left_paddle.y = scr_h / 2 - PADDLE_HEIGHT / 2.0;
    s->left_paddle.w = PADDLE_WIDTH;

    s->right_paddle.x = scr_w - PADDLE_WIDTH * 2;
    s->right_paddle.h = PADDLE_HEIGHT;
    s->right_paddle.y = scr_h / 2 - PADDLE_HEIGHT / 2.0;
    s->right_paddle.w = PADDLE_WIDTH;

    s->field_w = 200;
    s->field_h = 200;

    s->ball.x = scr_w / 2;
    s->ball.y = scr_h / 2;
}

int is_ball_collide(State* s) {
    Ball* b = &s->ball;
    Paddle* lp = &s->left_paddle;
    Paddle* rp = &s->right_paddle;

    Rectangle r0 = {b->x, b->y, 10, 10};
    Rectangle r1 = {lp->x, lp->y, lp->w, lp->h};
    Rectangle r2 = {rp->x, rp->y, rp->w, rp->h};

    return CheckCollisionRecs(r0, r1) || CheckCollisionRecs(r0, r2);
}

int is_ball_collide_with_paddle(State* s, Paddle* paddle) {
    Ball* b = &s->ball;
    Rectangle b_rect = {b->x, b->y, 10, 10};
    Rectangle p_rect = {paddle->x, paddle->y, paddle->w, paddle->h};
    return CheckCollisionRecs(b_rect, p_rect);
}

void update_ball(State* s) {
    Ball ball = s->ball;
    int ball_w = 10;
    float field_w = GetScreenWidth();
    float field_h = GetScreenHeight();
    Paddle* left_paddle = &s->left_paddle;
    Paddle* right_paddle = &s->right_paddle;
    float delta = GetFrameTime();

    ball.x += ball.vx * delta;
    ball.y += ball.vy * delta;

    bool collided = false;

    if (is_ball_collide_with_paddle(s, left_paddle)) {
        ball.x = left_paddle->x + left_paddle->w + 1;
        ball.vx = -ball.vx;
        collided = true;
    } else if (is_ball_collide_with_paddle(s, right_paddle)) {
        ball.x = right_paddle->x - 10 - 1;
        ball.vx = -ball.vx;
        collided = true;
    } else if (ball.y <= 0.0) {
        ball.y = 0.0;
        ball.vy = -ball.vy;
        collided = true;
    } else if (field_h <= ball.y + BALL_SIZE) {
        ball.y = field_h - BALL_SIZE;
        ball.vy = -ball.vy;
        collided = true;
    }

    if (collided) {
        ball.vy *= GetRandomValue(8, 12) / 10.0;
        ball.vx += 5;
    }

    if (ball.x <= left_paddle->x + left_paddle->w) {
        s->right_player_score += 1;
        ball.x = right_paddle->x - BALL_SIZE - 5;
        ball.y = right_paddle->y + right_paddle->h / 2.0 + 5.0;
        ball.vx = -200;
        ball.vy = GetRandomValue(-200, 200);
    }

    if (right_paddle->x < ball.x) {
        s->left_player_score += 1;
        ball.x = left_paddle->x + PADDLE_WIDTH + 5;
        ball.y = left_paddle->y + left_paddle->h / 2.0 + 5.0;
        ball.vx = 200;
        ball.vy = GetRandomValue(-200, 200);
    }

    s->ball = ball;
}

void update_panel(State* s, Paddle* p, int up, int down) {
    float field_w = GetScreenWidth();
    float field_h = GetScreenHeight();

    float delta = GetFrameTime();

    if (IsKeyDown(up))
        p->y -= PADDLE_SPEED * delta;
    if (IsKeyDown(down))
        p->y += PADDLE_SPEED * delta;

    if (p->y < 0) {
        p->y = 0;
    } else if (field_h - p->h < p->y) {
        p->y = field_h - p->h;
    }
}

void update_panels(State* s) {
    update_panel(s, &s->left_paddle, KEY_W, KEY_S);
    update_panel(s, &s->right_paddle, KEY_UP, KEY_DOWN);
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
    char score_string[10];

    while (!WindowShouldClose()) {
        update_state(&state);

        BeginDrawing();
        {
            ClearBackground(RAYWHITE);
            DrawRectangle(state.ball.x, state.ball.y, 10, 10, BLACK);
            DrawRectangle(10, state.left_paddle.y, 10, state.left_paddle.h,
                          BLACK);
            DrawRectangle(GetScreenWidth() - 20, state.right_paddle.y, 10,
                          state.right_paddle.h, BLACK);
            sprintf(fps_string, "%d", GetFPS());
            DrawText(fps_string, 0, 0, 16, BLACK);
            sprintf(fps_string, "%d : %d", state.left_player_score,
                    state.right_player_score);
            DrawText(fps_string, 0, 17, 16, BLACK);
        }
        EndDrawing();
    }

    return 0;
}
