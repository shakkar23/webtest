#include "game_logic.hpp"

void State::DrawRect(Rect outline, Color c) {
    if (0 <= outline.x && outline.x <= win_w && outline.y < win_h) {
        for (int i = outline.x; i < outline.x + outline.w; ++i) {
            int x = std::clamp(i, 0, win_w - 1);
            int y1 = std::clamp(outline.y, 0, win_h - 1);
            int y2 = std::clamp(outline.h + outline.y, 0, win_h - 1);

            // draw the two horizontal lines
            screen_data[y1 * win_w + x].r = c.r;
            screen_data[y1 * win_w + x].g = c.g;
            screen_data[y1 * win_w + x].b = c.b;
            screen_data[y1 * win_w + x].a = c.a;

            screen_data[y2 * win_w + x].r = c.r;
            screen_data[y2 * win_w + x].g = c.g;
            screen_data[y2 * win_w + x].b = c.b;
            screen_data[y2 * win_w + x].a = c.a;
        }
    }

    if (0 <= outline.y && outline.y < win_h && outline.x < win_w) {
        for (int i = outline.y; i <= outline.y + outline.h; ++i) {
            int y = std::clamp(i, 0, win_h - 1);

            int x1 = std::clamp(outline.x, 0, win_w - 1);
            int x2 = std::clamp(outline.w + outline.x, 0, win_w - 1);

            // draw the two vertical lines
            screen_data[y * win_w + x1].r = c.r;
            screen_data[y * win_w + x1].g = c.g;
            screen_data[y * win_w + x1].b = c.b;
            screen_data[y * win_w + x1].a = c.a;

            if (outline.x + outline.w < win_w) {
                screen_data[y * win_w + x2].r = c.r;
                screen_data[y * win_w + x2].g = c.g;
                screen_data[y * win_w + x2].b = c.b;
                screen_data[y * win_w + x2].a = c.a;
            }
        }
    }
}

void State::DrawRectFilled(Rect outline, Color c) {
    outline.x = std::clamp(outline.x, 0, win_w - 1);
    outline.y = std::clamp(outline.y, 0, win_h - 1);
    outline.w = std::clamp(outline.w, 0, win_w - outline.x);
    outline.h = std::clamp(outline.h, 0, win_h - outline.y);

    // render
    for (int y = outline.y; y <= outline.y + outline.h; ++y) {
        for (int x = outline.x; x <= outline.x + outline.w; ++x) {
            screen_data[y * win_w + x].r = c.r;
            screen_data[y * win_w + x].g = c.g;
            screen_data[y * win_w + x].b = c.b;
            screen_data[y * win_w + x].a = c.a;
        }
    }
}

constexpr auto das = 9.0f / 60.0f;
constexpr auto arr = 0.0f / 60.0f;

void State::logic(float dt) {
    if (getKey('w').pressed) {
        this->game.p1_game.process_movement(this->game.p1_game.current_piece, Movement::SonicDrop);

        Move move = Move(this->game.p1_game.current_piece, false);
        Move null_move = Move(this->game.p2_game.current_piece, true);

        this->game.set_move(0, move);
        this->game.set_move(1, null_move);

        this->game.play_moves();
    }

    static float right_acc = 0.0;
    if (getKey('a').held || getKey('a').pressed) {
        if (right_acc == 0.0f) {  // das
            this->game.p1_game.process_movement(this->game.p1_game.current_piece, Movement::Left);
        } else if (right_acc >= das) {  // arr is once per frame but whatever
            this->game.p1_game.process_movement(this->game.p1_game.current_piece, Movement::Left);
        }
        right_acc += dt;
    } else {
        right_acc = 0.0f;
    }

    if (getKey('s').held || getKey('s').pressed) {
        this->game.p1_game.process_movement(this->game.p1_game.current_piece, Movement::SonicDrop);
    }

    static float left_acc = 0.0f;
    if (getKey('d').held) {
        if (left_acc == 0.0f) {  // das
            this->game.p1_game.process_movement(this->game.p1_game.current_piece, Movement::Right);
        } else if (left_acc >= das) {  // arr is once per frame but whatever
            this->game.p1_game.process_movement(this->game.p1_game.current_piece, Movement::Right);
        }
        left_acc += dt;
    } else {
        left_acc = 0.0f;
    }

    if (getKey(Up).pressed) {
        // do hold
        this->game.p1_game.do_hold();
    }
    if (getKey(Left).pressed) {
        this->game.p1_game.process_movement(this->game.p1_game.current_piece, Movement::RotateCounterClockwise);
    }
    if (getKey(Down).pressed) {
        // we dont support 180s yet
    }
    if (getKey(Right).pressed) {
        this->game.p1_game.process_movement(this->game.p1_game.current_piece, Movement::RotateClockwise);
    }

    if (this->game.game_over) {
        this->game = VersusGame();
    }

    this->prev_keyboard = this->keyboard;
}

void State::render() {
    // clear
    for (std::size_t i = 0; i < this->screen_data.size(); i += 1) {
        this->screen_data[i].r = 0;    // R value
        this->screen_data[i].g = 0;    // G value
        this->screen_data[i].b = 0;    // B value
        this->screen_data[i].a = 255;  // A value
    }

    Rect resting_eye_location = getInnerRect({.x = 0, .y = 0, .w = this->win_w, .h = this->win_h}, 1.0 / 1.0);
    // move to where the eyes rest according to the thirds rule
    // we will render everything in here
    resting_eye_location.y = int((resting_eye_location.y - 0) * 0.33);

    const Rect board_area = getInnerRect(resting_eye_location, 10.0 / 20.0);

    // render the ghost piece
    Piece ghost = this->game.p1_game.current_piece;
    this->game.p1_game.process_movement(ghost, Movement::SonicDrop);
    for (auto& mino : ghost.minos) {
        int x = mino.x + ghost.position.x;
        int y = 19 - (mino.y + ghost.position.y);

        float cell_length = board_area.w / 10.0;
        int cell_x = board_area.x + cell_length * x;
        int cell_y = board_area.y + cell_length * y;
        this->DrawRectFilled({.x = cell_x, .y = cell_y, .w = int(cell_length), .h = int(cell_length)}, light_gray);
    }

    // render the current piece
    for (auto& mino : this->game.p1_game.current_piece.minos) {
        int x = mino.x + this->game.p1_game.current_piece.position.x;
        int y = 19 - (mino.y + this->game.p1_game.current_piece.position.y);

        float cell_length = board_area.w / 10.0;
        int cell_x = board_area.x + cell_length * x;
        int cell_y = board_area.y + cell_length * y;
        this->DrawRectFilled({.x = cell_x, .y = cell_y, .w = int(cell_length), .h = int(cell_length)}, blue);
    }

    // render the board cells
    for (int x = 0; x < 10; ++x)
        for (int y = 0; y < 20; ++y) {
            float cell_length = board_area.w / 10.0;
            int cell_x = board_area.x + cell_length * x;
            int cell_y = board_area.y + cell_length * (19 - y);
            if (this->game.p1_game.board.get(x, y))
                this->DrawRectFilled({.x = cell_x, .y = cell_y, .w = int(cell_length), .h = int(cell_length)}, gray);
        }

    // render the queue
    const Rect queue_area = {
        .x = board_area.x + board_area.w,
        .y = board_area.y,
        // three real cells wide
        .w = int(board_area.w / (board_area.w / 10.0f) * 3.0f),
        .h = board_area.h /* / (board_area.h / 20.0)*/
    };

    for (int piece_i = 0; piece_i < game.p1_game.queue.size(); ++piece_i) {
        const Rect drawing_area = {
            .x = queue_area.x,
            .y = int(((float)queue_area.h / game.p1_game.queue.size()) * piece_i) + queue_area.y,
            .w = queue_area.w,
            .h = queue_area.h,
        };

        Piece queue_piece = game.p1_game.queue[piece_i];
        for (auto& mino : queue_piece.minos) {
            int x = mino.x;
            int y = mino.y;
            float cell_length = drawing_area.w / 5.0f; // five cells in width were provided
            int cell_x = drawing_area.x + cell_length * (x + 2);
            int cell_y = drawing_area.y + cell_length * (5 - (y + 2));
            this->DrawRectFilled({.x = cell_x, .y = cell_y, .w = int(cell_length), .h = int(cell_length)}, red);
        }

        for (int x = 0; x < 5; ++x) {
            for (int y = 0; y < 5; ++y) {
                float cell_length = drawing_area.w / 5.0f;  // five cells in width were provided
                int cell_x = drawing_area.x + cell_length * x;
                int cell_y = drawing_area.y + cell_length * y;
                this->DrawRect({.x = cell_x, .y = cell_y, .w = int(cell_length), .h = int(cell_length)}, blue);
            }
        }

        // this->DrawRect(drawing_area, green);
    }
    // this->DrawRect(queue_area, green);


    // render the hold
    const Rect hold_area = {
        .x = board_area.x - int((board_area.w / 10.0f) * 3.0f),  // minus three cells wide
        .y = board_area.y,
        // three real cells wide
        .w = int(board_area.w / (board_area.w / 10.0f) * 3.0f),
        .h = int(board_area.h / 20.0f * 3.0f)
    };

    if(this->game.p1_game.hold.has_value())
        for (auto& mino : this->game.p1_game.hold.value().minos) {
            int x = mino.x;
            int y = mino.y;
            float cell_length = hold_area.w / 5.0f;  // five cells in width were provided
            int cell_x = hold_area.x + cell_length * (x + 2);
            int cell_y = hold_area.y + cell_length * (5 - (y + 2));
            this->DrawRectFilled({.x = cell_x, .y = cell_y, .w = int(cell_length), .h = int(cell_length)}, red);
        }

    // render the board grid
    for (int x = 0; x < 10; ++x)
        for (int y = 0; y < 20; ++y) {
            float cell_length = board_area.w / 10.0;
            int cell_x = board_area.x + cell_length * x;
            int cell_y = board_area.y + cell_length * y;
            this->DrawRect({.x = cell_x, .y = cell_y, .w = int(cell_length), .h = int(cell_length)}, gray);
        }

    // this->DrawRect(board_area, green);
}

State::Key State::getKey(char key) {
    Key keyState{};

    auto cur = std::find(keyboard.begin(), keyboard.end(), key) != keyboard.end();

    auto prev = std::find(prev_keyboard.begin(), prev_keyboard.end(), key) != prev_keyboard.end();

    keyState.held = cur && prev;
    keyState.pressed = justPressed(prev, cur);
    keyState.released = !cur && prev;

    return keyState;
}

bool State::justPressed(bool prevInput, bool input) { return (!prevInput && input); }
