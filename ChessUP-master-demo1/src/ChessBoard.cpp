#include "../include/ChessBoard.h"

#include "../include/GameState.h"
#include "../include/Piece.h"
#include "../include/States.h"
#include "../include/Pawn.h"


/// The window we'll be rendering to
SDL_Window* g_Window = nullptr;

/// The window renderer
SDL_Renderer* g_Renderer = nullptr;

/// Event handler
SDL_Event e;

/// Font
TTF_Font* g_Font;

/// Button objects
Button g_Button[ (int) ButtonSprite::BUTTON_SPRITE_TOTAL ];
Button m_Button[ TOTAL_MAIN_MENU + TOTAL_PAUSE_MENU ];

/// Button sprites
SDL_Rect g_SpriteClips[ (int) ButtonSprite::BUTTON_SPRITE_TOTAL ];
SDL_Rect m_SpriteClips[ TOTAL_MAIN_MENU + TOTAL_PAUSE_MENU ];
Texture t_ButtonSpriteSheetTexture;

/// Scene textures
Texture t_Board;
Texture t_MainMenu;
Texture t_Background;

Texture t_WhitePieces[7];
Texture t_BlackPieces[7];
Texture t_PossibleMoves;
Texture t_PieceSelected;
Texture t_EndGame[3];

Texture t_Text;

/// Stack save movement
std::stack<int> x_position_stack;
std::stack<int> y_position_stack;
std::stack<int> index_stack;
std::stack<bool> color_stack;
std::stack<bool> kingCastling_stack;


ChessBoard::ChessBoard()
{
    x_board_ = (SCREEN_WIDTH - CELL_SIZE * 8) / 2;
    y_board_ = (SCREEN_HEIGHT - CELL_SIZE * 8) / 2;

    focus_ = {-1, -1};
    focusedPiece_ = nullptr;
}

ChessBoard::~ChessBoard()
{
}

/*********** General Function ***********/

bool ChessBoard::init()
{
    initSDL(g_Window, g_Renderer, SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    return ((g_Window != nullptr) && (g_Renderer != nullptr));
}

bool ChessBoard::loadImage()
{
    bool success = true;

    // Loads the texture
    if(!t_Board.loadFromFile(chess_board))
        success = false;

    if(!t_MainMenu.loadFromFile(main_menu))
        success = false;

    if(!t_Background.loadFromFile(chess_background))
        success = false;

    if(!t_PieceSelected.loadFromFile(piece_selected))
        success = false;

    if(!t_PossibleMoves.loadFromFile(possible_moves))
        success = false;

    // End game textures
    // White wins
    if(!t_EndGame[(int) GameResult::WHITE_WINS].loadFromFile(white_win))
        success = false;

    // Black wins
    if(!t_EndGame[(int) GameResult::BLACK_WINS].loadFromFile(black_win))
        success = false;

    // Draw
    if(!t_EndGame[(int) GameResult::DRAW].loadFromFile(draw))
        success = false;

    // Chess pieces

    // White pieces

    if(!t_WhitePieces[(int) PieceName::KING].loadFromFile(king_white))
        success = false;

    if(!t_WhitePieces[(int) PieceName::QUEEN].loadFromFile(queen_white))
        success = false;

    if(!t_WhitePieces[(int) PieceName::BISHOP].loadFromFile(bishop_white))
        success = false;

    if(!t_WhitePieces[(int) PieceName::KNIGHT].loadFromFile(knight_white))
        success = false;

    if(!t_WhitePieces[(int) PieceName::ROOK].loadFromFile(rook_white))
        success = false;

    if(!t_WhitePieces[(int) PieceName::PAWN].loadFromFile(pawn_white))
        success = false;

    // Black pieces

    if(!t_BlackPieces[(int) PieceName::KING].loadFromFile(king_black))
        success = false;

    if(!t_BlackPieces[(int) PieceName::QUEEN].loadFromFile(queen_black))
        success = false;

    if(!t_BlackPieces[(int) PieceName::BISHOP].loadFromFile(bishop_black))
        success = false;

    if(!t_BlackPieces[(int) PieceName::KNIGHT].loadFromFile(knight_black))
        success = false;

    if(!t_BlackPieces[(int) PieceName::ROOK].loadFromFile(rook_black))
        success = false;

    if(!t_BlackPieces[(int) PieceName::PAWN].loadFromFile(pawn_black))
        success = false;

    // Loads button sprite

    if(!t_ButtonSpriteSheetTexture.loadFromFile(menu_box))
    {
        logSDLError(std::cout, "Failed to load button sprite texture", SDL_ERROR, true);
        success = false;
    }
    else
    {
        // Sets sprite
        for(int i = 0; i < (int) ButtonSprite::BUTTON_SPRITE_TOTAL; i++)
        {
            g_SpriteClips[ i ].x = 0;
            g_SpriteClips[ i ].y = i * MENU_HEIGHT;
            g_SpriteClips[ i ].w = MENU_WIDTH;
            g_SpriteClips[ i ].h = MENU_HEIGHT;
        }

        // Sets button position
        g_Button[ 0 ].setPosition(x_board_ - BOARD_PADDING, y_board_ - BOARD_PADDING);
        g_Button[ 1 ].setPosition(x_board_ - BOARD_PADDING + MENU_WIDTH, y_board_ - BOARD_PADDING);
        g_Button[ 2 ].setPosition(x_board_ - BOARD_PADDING, y_board_ - BOARD_PADDING + MENU_HEIGHT);
        g_Button[ 3 ].setPosition(x_board_ - BOARD_PADDING + MENU_WIDTH, y_board_ - BOARD_PADDING + MENU_HEIGHT);
        g_Button[ 4 ].setPosition(x_board_ - BOARD_PADDING, y_board_ - BOARD_PADDING);
        g_Button[ 5 ].setPosition(x_board_ - BOARD_PADDING + MENU_WIDTH, y_board_ - BOARD_PADDING);
        g_Button[ 6 ].setPosition(x_board_ - BOARD_PADDING, y_board_ - BOARD_PADDING + MENU_HEIGHT);
        g_Button[ 7 ].setPosition(x_board_ - BOARD_PADDING + MENU_WIDTH, y_board_ - BOARD_PADDING + MENU_HEIGHT);
    }

    m_Button[0].setText("Play PVP");
    m_Button[1].setText("Play CPU");
    m_Button[2].setText("Edit mode");
    m_Button[3].setText("Quit");
    m_Button[4].setText("Continue");
    m_Button[5].setText("Save");
    m_Button[6].setText("Load");
    m_Button[7].setText("Main Menu");

    for(int i = 0; i < TOTAL_MAIN_MENU; i++)
    {
        m_SpriteClips[i].x = m_Button[i].x_button_;
        m_SpriteClips[i].y = m_Button[i].y_button_ + MENU_HEIGHT * i + OFFSET * i;
        m_SpriteClips[i].w = MENU_WIDTH;
        m_SpriteClips[i].h = MENU_HEIGHT;

        m_Button[i].setPosition(m_Button[i].x_button_, m_Button[i].y_button_ + MENU_HEIGHT * i + OFFSET * i);
        m_Button[i].setColor(Color::BLACK_COLOR);
    }

    for(int i = 4; i < 4 + TOTAL_PAUSE_MENU; i++)
    {
        m_SpriteClips[i].x = m_Button[i].x_button_;
        m_SpriteClips[i].y = m_Button[i].y_button_ + MENU_HEIGHT * (i - 4) + OFFSET * (i - 4);
        m_SpriteClips[i].w = MENU_WIDTH;
        m_SpriteClips[i].h = MENU_HEIGHT;

        m_Button[i].setPosition(m_Button[i].x_button_, m_Button[i].y_button_ + MENU_HEIGHT * (i - 4) + OFFSET * (i - 4));
        m_Button[i].setColor(Color::BLACK_COLOR);
    }


    // Loads font
    g_Font = TTF_OpenFont(text_fonts, MENU_TEXT_SIZE);
    if(g_Font == nullptr)
        logSDLError(std::cout, "Load Font", TTF_ERROR, true);

    // Check success
    if(!success)
        logSDLError(std::cout, "Failed to load texture", SDL_ERROR, true);

    return success;
}

void ChessBoard::close()
{
    // Deallocates memory
    t_ButtonSpriteSheetTexture.freeTexture();
    t_Board.freeTexture();
    t_MainMenu.freeTexture();
    t_Background.freeTexture();
    t_PossibleMoves.freeTexture();
    t_PieceSelected.freeTexture();
    t_Text.freeTexture();

    for(int i = 0; i < 3; i++)
    {
        t_EndGame[i].freeTexture();
    }

    // Free chess pieces
    for(int i = 0; i < 7; i++)
    {
        t_BlackPieces[i].freeTexture();
        t_WhitePieces[i].freeTexture();
    }

    // Destroy window
    SDL_DestroyWindow(g_Window);
    SDL_DestroyRenderer(g_Renderer);
    g_Window = NULL;
    g_Renderer = NULL;

    // Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();
}

/*********** Render Function ***********/

void ChessBoard::renderChessBoard()
{
#ifndef BOARD
    // Rect draw padding of board
    SDL_Rect outRect;
    // Set padding of board
    outRect.x = x_board_ - BOARD_PADDING;
    outRect.y = y_board_ - BOARD_PADDING;
    outRect.w = CELL_SIZE * 8 + BOARD_PADDING * 2;
    outRect.h = CELL_SIZE * 8 + BOARD_PADDING * 2;
    // Set padding color
    SDL_SetRenderDrawColor(g_Renderer, 64, 64, 64, 255);
    // Render padding of board
    SDL_RenderFillRect(g_Renderer, &outRect);

    // Rect draw each cell
    SDL_Rect cell;
    for (int iRow = 0; iRow < 8; iRow++)
    {
        for (int iCol = 0; iCol < 8; iCol++)
        {
            // Set one cell
            cell.x = x_board_ + CELL_SIZE * iCol;
            cell.y = y_board_ + CELL_SIZE * iRow;
            cell.w = CELL_SIZE;
            cell.h = CELL_SIZE;

            // Set color of each cell
            if((iRow + iCol) % 2 == 0)
                SDL_SetRenderDrawColor(g_Renderer, 0, 128, 0, 255);
            else
                SDL_SetRenderDrawColor(g_Renderer, 255, 255, 255, 255);

            // Render one cell
            SDL_RenderFillRect(g_Renderer, &cell);
        }
    }
#else
    t_Board.render(x_board_ - BOARD_PADDING, y_board_ - BOARD_PADDING, BOARD_HEIGHT, BOARD_WIDTH);
#endif // BOARD
}

void ChessBoard::renderPieceOnBoard(PieceName piece, bool color, int x_pos, int y_pos)
{
    if( (x_pos < 0) || (y_pos < 0) )
        return;
    if(color)
        t_WhitePieces[(int) piece].render(indexToPixel(x_pos, true), indexToPixel(y_pos, false), CELL_SIZE, CELL_SIZE);
    else
        t_BlackPieces[(int) piece].render(indexToPixel(x_pos, true), indexToPixel(y_pos, false), CELL_SIZE, CELL_SIZE);
}

void ChessBoard::renderAllPieces(States* states)
{
    for(int i = 0; i < 16; i++)
    {
        // Render white pieces
        renderPieceOnBoard(states->whitePieces_[i]->getName(), true, states->whitePieces_[i]->getPositionX(), states->whitePieces_[i]->getPositionY());

        // Render black pieces
        renderPieceOnBoard(states->blackPieces_[i]->getName(), false, states->blackPieces_[i]->getPositionX(), states->blackPieces_[i]->getPositionY());
    }
}

void ChessBoard::renderOnePiece(int x_pos, int y_pos)
{
#ifndef BOARD
    // Set rect to render in
    SDL_Rect rect = {x_pos, y_pos, CELL_SIZE, CELL_SIZE};

    // Render rect on the board
    SDL_RenderFillRect(g_Renderer, &rect);
#else
    t_PieceSelected.render(x_pos, y_pos, CELL_SIZE, CELL_SIZE);
#endif // BOARD
}

void ChessBoard::renderPossibleMoves(States *states)
{
    if(focusedPiece_)
    {
        if(focusedPiece_->getName() != PieceName::EMPTY)
        {
            for(int iRow = 0; iRow < 8; iRow++)
            {
                for(int iCol = 0; iCol < 8; iCol++)
                {
                    if(states->isPositionValid(focusedPiece_, iRow, iCol))
                    {
                        t_PossibleMoves.render(indexToPixel(iRow, true), indexToPixel(iCol, false), CELL_SIZE, CELL_SIZE);
                    }
                }
            }
        }
    }
}

/*********** Other Function ***********/

void ChessBoard::updateFocus(int x_pos, int y_pos)
{
    // Check outside the edges of the board
    if( (x_pos < x_board_) || (x_pos > x_board_ + CELL_SIZE * 8) || (y_pos < y_board_) || (y_pos > y_board_ + CELL_SIZE * 8) )
    {
        focus_.x = -1;
        focus_.y = -1;
        return;
    }
    focus_.x = (int) (x_pos - x_board_) / CELL_SIZE;
    focus_.y = (int) (y_pos - y_board_) / CELL_SIZE;
}

int ChessBoard::indexToPixel(int index, bool xORy)
{
    if( (index < 0) || (index > 7) )
    {
        logSDLError(std::cout, "Invalid index", SDL_ERROR, true);
        return -1;
    }
    if(xORy)
        return (index * CELL_SIZE + x_board_);
    else
        return (index * CELL_SIZE + y_board_);
}

bool ChessBoard::choosePieceTurn(GameState *gm, States* states)
{
    /******************************************************************* todo *******************************************************************/
    return true;
}

bool ChessBoard::checkMovement(States* states)
{
    bool move = false;
    if(focusedPiece_ != nullptr)
    {
        // Check focused piece can move or not
        if(focusedPiece_->getName() != PieceName::EMPTY)
        {
            move = states->isMove(focusedPiece_, states->getPieceTurn(), focus_.x, focus_.y);
        }
    }
    return move;
}

void ChessBoard::editBoard(GameState *gm, States* states)
{
    bool done = false;
    states->killAllPieces();
    int indexPiece = 0;
    bool isWhite = false;
    int x = -1, y = -1;

    while(!done)
    {
        while(SDL_PollEvent(&e))
        {
            if(e.type == SDL_QUIT)
            {
                gm->setGameState(GameMode::GAME_MODE_QUIT);
                done = true;
                break;
            }
            else if(e.type == SDL_MOUSEMOTION)
            {
                SDL_GetMouseState(&x, &y);
            }
            else if(e.type == SDL_MOUSEBUTTONDOWN)
            {
                // Gets mouse position
                SDL_GetMouseState(&x, &y);
                // Updates the index of the focused matrix
                updateFocus((int) x, (int) y);
                if(isWhite)
                    states->setPiece(states->whitePieces_[indexPiece], focus_.x, focus_.y);
                else
                    states->setPiece(states->blackPieces_[indexPiece], focus_.x, focus_.y);
            }
            else if(e.type == SDL_KEYDOWN)
            {
                switch (e.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    gm->setGameState(GameMode::GAME_MODE_MAIN_MENU);
                    done = true;
                    break;
                case SDLK_UP:
                    isWhite = !isWhite;
                    break;
                case SDLK_DOWN:
                    isWhite = !isWhite;
                    break;
                case SDLK_LEFT:
                    if(indexPiece > 0)
                        indexPiece --;
                    else
                        indexPiece = 15;
                    break;
                case SDLK_RIGHT:
                    if(indexPiece < 15)
                        indexPiece ++;
                    else
                        indexPiece = 0;
                    break;
                case SDLK_RETURN:
                    done = true;
                    break;
                default:
                    break;
                }
            }

            // Clear screen
            SDL_SetRenderDrawColor(g_Renderer, 0xFF, 0xFF, 0xFF, 0xFF);
            SDL_RenderClear(g_Renderer);

            // Render chess background
            t_Background.renderBackground();
            // Render board
            renderChessBoard();
            // Render piece on board
            renderAllPieces(states);

            if(isWhite)
                t_WhitePieces[(int) states->whitePieces_[indexPiece]->getName()].render(x - CELL_SIZE/2, y - CELL_SIZE/2);
            else
                t_BlackPieces[(int) states->blackPieces_[indexPiece]->getName()].render(x - CELL_SIZE/2, y - CELL_SIZE/2);

            // Update screen
            updateScreen(g_Window, g_Renderer);
        }
    }
    return;
}
