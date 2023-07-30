#include <math.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>

#define PADDLE_WIDTH 0.03   // 3vw
#define PADDLE_HEIGHT 0.20  // 20vh
#define PADDLE_SPEED 1.0    // 1vw per sec
#define BALL_SIZE 0.03      // 1vw
#define SCREEN_WIDTH 1.0
#define SCREEN_HEIGHT 1.0
#define MAX_Y_VELOCITY 2.0
#define MAX_X_VELOCITY 2.0
#define FONT_SIZE 16

typedef enum {
    Running,
    Left_Win_Screen,
    Right_Win_Screen,
    Main_Menu,
} Step;

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
    Ball ball;
    Paddle left_paddle;
    Paddle right_paddle;
    int left_player_score;
    int right_player_score;
    Step step;
    bool pause;
} State;

void init_state(State* s) {
    s->left_paddle.x = PADDLE_WIDTH * 2.0;
    s->left_paddle.h = PADDLE_HEIGHT;
    s->left_paddle.y = SCREEN_HEIGHT / 2.0 - PADDLE_HEIGHT / 2.0;
    s->left_paddle.w = PADDLE_WIDTH;

    s->right_paddle.x = SCREEN_WIDTH - PADDLE_WIDTH * 3.0;
    s->right_paddle.h = PADDLE_HEIGHT;
    s->right_paddle.y = SCREEN_HEIGHT / 2.0 - PADDLE_HEIGHT / 2.0;
    s->right_paddle.w = PADDLE_WIDTH;

    s->ball.x = SCREEN_WIDTH / 2.0;
    s->ball.y = SCREEN_HEIGHT / 2.0;

    s->step = Running;
    s->pause = true;
}

int get_screen_aspect_ratio() {
    return GetScreenWidth() / GetScreenHeight();
}

int is_ball_collide_with_paddle(Ball* b, Paddle* p) {
    int aspect_ratio = get_screen_aspect_ratio();

    if (p->x + p->w < b->x)
        return false;

    if (p->y + p->h < b->y)
        return false;

    if (b->x + BALL_SIZE < p->x)
        return false;

    if (b->y + BALL_SIZE * aspect_ratio < p->y)
        return false;

    return true;
}

void update_vy_after_paddle_collision(Ball* b, Paddle* p) {
    float paddle_center_y = p->y + p->h / 2.0;
    float ball_center_y = b->y + BALL_SIZE / 2.0;
    float offset = sqrtf(fabs(paddle_center_y - ball_center_y));
    if (ball_center_y < paddle_center_y)
        b->vy -= offset;
    else
        b->vy += offset;
}

void update_ball(State* s) {
    Ball ball = s->ball;
    Paddle* left_paddle = &s->left_paddle;
    Paddle* right_paddle = &s->right_paddle;
    float delta = GetFrameTime();

    int aspect_ratio = get_screen_aspect_ratio();

    ball.x += ball.vx * aspect_ratio * delta;
    ball.y += ball.vy * delta;

    bool collided = false;

    if (is_ball_collide_with_paddle(&ball, left_paddle)) {
        ball.x = left_paddle->x + left_paddle->w;
        ball.vx = -ball.vx;
        update_vy_after_paddle_collision(&ball, left_paddle);
        collided = true;
    } else if (is_ball_collide_with_paddle(&ball, right_paddle)) {
        ball.x = right_paddle->x - BALL_SIZE;
        ball.vx = -ball.vx;
        update_vy_after_paddle_collision(&ball, right_paddle);
        collided = true;
    } else if (ball.y <= 0.0) {
        ball.y = 0.0;
        ball.vy = -ball.vy;
        collided = true;
    } else if (SCREEN_HEIGHT <= ball.y + BALL_SIZE * aspect_ratio) {
        ball.y = SCREEN_HEIGHT - BALL_SIZE * aspect_ratio;
        ball.vy = -ball.vy;
        collided = true;
    } else if (ball.x <= 0.0) {
        ball.x = 0.0;
        ball.vx = -ball.vx;
        collided = true;
    } else if (SCREEN_WIDTH <= ball.x + BALL_SIZE) {
        ball.x = SCREEN_WIDTH - BALL_SIZE;
        ball.vx = -ball.vx;
        collided = true;
    }

    if (collided) {
        ball.vy *= GetRandomValue(95, 110) / 100.0;
        ball.vx *= GetRandomValue(95, 110) / 100.0;
    } else if (ball.x < left_paddle->x + left_paddle->w) {
        // s->step = Right_Win_Screen;
        s->right_player_score += 1;
        ball.x = right_paddle->x - PADDLE_WIDTH;
        ball.y = right_paddle->y + right_paddle->h / 2.0;
        ball.vx = GetRandomValue(20, 40) / 100.0;
        ball.vy = GetRandomValue(-40, 40) / 100.0;
    } else if (right_paddle->x < ball.x) {
        // s->step = Left_Win_Screen;
        s->left_player_score += 1;
        ball.x = left_paddle->x + left_paddle->w;
        ball.y = left_paddle->y + left_paddle->h / 2.0 + BALL_SIZE / 2.0;
        ball.vx = -GetRandomValue(20, 40) / 100.0;
        ball.vy = GetRandomValue(-40, 40) / 100.0;
    }

    s->ball = ball;
}

void update_paddle(State* s, Paddle* p, int up, int down) {
    float delta = GetFrameTime();

    if (IsKeyDown(up))
        p->y -= PADDLE_SPEED * delta;
    if (IsKeyDown(down))
        p->y += PADDLE_SPEED * delta;

    if (p->y < 0) {
        p->y = 0;
    } else if (SCREEN_HEIGHT - p->h < p->y) {
        p->y = SCREEN_HEIGHT - p->h;
    }
}

void update_paddles(State* s) {
    update_paddle(s, &s->left_paddle, KEY_W, KEY_S);
    update_paddle(s, &s->right_paddle, KEY_UP, KEY_DOWN);
}

void update_state(State* s) {
    if (s->step == Running) {
        if (IsKeyPressed(KEY_P)) {
            s->pause = !s->pause;
        }
        if (!s->pause) {
            update_paddles(s);
            update_ball(s);
        }
    } else if (s->step == Main_Menu) {
    } else if (s->step == Left_Win_Screen) {
    }
}

Rectangle get_real_paddle_dimentions(Paddle* p) {
    Rectangle r;
    int scr_w = GetScreenWidth();
    int scr_h = GetScreenHeight();
    r.x = p->x * scr_w;
    r.y = p->y * scr_h;
    r.width = p->w * scr_w;
    r.height = p->h * scr_h;
    return r;
}

Rectangle get_real_ball_rect(Ball* b) {
    Rectangle r;
    int scr_w = GetScreenWidth();
    int scr_h = GetScreenHeight();
    r.x = b->x * scr_w;
    r.y = b->y * scr_h;
    r.width = BALL_SIZE * scr_w;
    r.height = BALL_SIZE * scr_w;
    return r;
}

char game_is_paused_text[] = "Paused";

void draw(State* s) {
    if (s->step == Left_Win_Screen) {
        // asdfdsaf
    }

    if (s->pause) {
        int len = MeasureText(game_is_paused_text, FONT_SIZE);
        int x = GetScreenWidth() - len - FONT_SIZE;
        int y = FONT_SIZE;
        DrawText(game_is_paused_text, x, y, FONT_SIZE, WHITE);
    }

    char string_buffer[10];

    int scr_w = GetScreenWidth();
    int scr_h = GetScreenHeight();

    Rectangle left_paddle_rect = get_real_paddle_dimentions(&s->left_paddle);
    Rectangle right_paddle_rect = get_real_paddle_dimentions(&s->right_paddle);
    Rectangle ball_rect = get_real_ball_rect(&s->ball);

    DrawRectangleRec(left_paddle_rect, RAYWHITE);
    DrawRectangleRec(right_paddle_rect, RAYWHITE);
    DrawRectangleRec(ball_rect, RAYWHITE);

    sprintf(string_buffer, "%d", GetFPS());
    DrawText(string_buffer, 0, 0, FONT_SIZE, RAYWHITE);
    sprintf(string_buffer, "%d : %d", s->left_player_score, s->right_player_score);
    int score_text_length = MeasureText(string_buffer, FONT_SIZE);
    DrawText(string_buffer, GetScreenWidth() / 2 - score_text_length / 2, 17, FONT_SIZE, RAYWHITE);
}

int main() {
    InitWindow(800, 400, "pong");
    SetWindowState(FLAG_WINDOW_RESIZABLE
                   | FLAG_VSYNC_HINT
    );

    State state;
    init_state(&state);

    state.ball.vx = 0.3;
    state.ball.vy = 0.3;

    while (!WindowShouldClose()) {
        update_state(&state);

        BeginDrawing();
        {
            ClearBackground(BLACK);
            draw(&state);
        }
        EndDrawing();
        // WaitTime(0.016);
    }

    return 0;
}
