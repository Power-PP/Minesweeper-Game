#include <SFML/Graphics.hpp>
#include <cmath>
#include <sstream>
#include <ctime>
#include <iostream>
#define  MAX 100

/*
0 is empty
1-8 is number
9 is mine
10 is unrevealed square
11 is flag
*/

sf::Color golden_compass(255, 202, 46);

// Definitions
float logo_animation;
int width = 5, height = 5, gameTime = 0, menustage = 0, player_moves = 0, do_the_magic = 0;
int mouse_button = 0; // 0 is none, 1 is left-click, 2 is middle-click, 3 is right-click, 4 is magic button
int squareNumber, mineNumber, unrevealed, flagNum, minesLeft, faceSize, customw, customh, customm, tutorialw, tutorialh;
int squareSize = 32;
int gameBorder_top = 4, gameBorder_right = 1, gameBorder_bottom = 1, gameBorder_left = 1;
int board[MAX * MAX] = { 0 };
int boardRevealed[MAX * MAX] = { 0 }; // -2 is point of start, -1 is point of dead, 0 is unrevealed, 1 is revealed, 2 is flagged, 3 is not sure, 4 is magic
bool first_click = true, gameOver = false, won = false, show_moves = false, show_magic = false;
bool mark_flag = true, magic_option = true, display_size = true;

// Funtions meta:
int nearMines(int x, int y);
void countSurrounding(int x, int y);
void placeMines(int first_click_poz);
bool revealed(int x, int y);
void display(int x, int y, int firstX, int firstY);
bool reveal(int x, int y);
void reset();
bool show_surroundings(int x, int y);
double abs(double a);
float lerp(float a, float b, float c);

// All Functions here:
int nearMines(int x, int y) {
    if (x >= 0 && x < width && y >= 0 && y <= height) {
        if (board[y * width + x] == 9) return 1;
        else return 0;
    } else return 0;
}

void countSurrounding(int x, int y) {
    if (board[y * width + x] == 9) return;
    unsigned int result = 0;
    if (nearMines(x + 1, y)) result++;
    if (nearMines(x - 1, y)) result++;
    if (nearMines(x, y - 1)) result++;
    if (nearMines(x, y + 1)) result++;
    if (nearMines(x + 1, y + 1)) result++;
    if (nearMines(x + 1, y - 1)) result++;
    if (nearMines(x - 1, y + 1)) result++;
    if (nearMines(x - 1, y - 1)) result++;
    board[y * width + x] = result;
}

void placeMines(int first_click_poz) {
    int count = 0;
    while (count < mineNumber) {
        int randPosition = rand() % squareNumber;
        if (board[randPosition] != 9 && board[randPosition] != -2) {
            board[randPosition] = 9;
            ++count;
        }
    }
    if (board[first_click_poz] == -2)
        board[first_click_poz] = 0;
}

bool revealed(int x, int y) {
    if (x < 0 || x >= width || y < 0 || y >= height) return true;
    if (boardRevealed[y * width + x] == 1 || boardRevealed[y * width + x] == 2) return true;
    return false;
}

void display(int x, int y, int firstX, int firstY) {
    if (x < 0 || x >= width || y < 0 || y >= height) return;
    int i = y * width + x;
    if (boardRevealed[i] == 1 || boardRevealed[i] == 2 || board[i] == 9) return;
    boardRevealed[i] = 1;
    --unrevealed;
    if (board[i] == 0) {
        if (!revealed(x + 1, y)) {
            display(x + 1, y, x, y);
        }
        if (!revealed(x - 1, y)) {
            display(x - 1, y, x, y);
        }
        if (!revealed(x, y + 1)) {
            display(x, y + 1, x, y);
        }
        if (!revealed(x, y - 1)) {
            display(x, y - 1, x, y);
        }
        if (!revealed(x + 1, y + 1)) {
            display(x + 1, y + 1, x, y);
        }
        if (!revealed(x + 1, y - 1)) {
            display(x + 1, y - 1, x, y);
        }
        if (!revealed(x - 1, y + 1)) {
            display(x - 1, y + 1, x, y);
        }
        if (!revealed(x - 1, y - 1)) {
            display(x - 1, y - 1, x, y);
        }
    }
}

bool reveal(int x, int y) {
    if (x < 0 || x >= width || y < 0 || y >= height) return false;
    int i = y * width + x;
    if (boardRevealed[i] == 0 || boardRevealed[i] == 3) {
        boardRevealed[i] = 1;
        --unrevealed;
        player_moves++;
        //magic();
        if (board[i] == 0) {
            display(x + 1, y, x, y);
            display(x - 1, y, x, y);
            display(x, y + 1, x, y);
            display(x, y - 1, x, y);
            display(x + 1, y - 1, x, y);
            display(x + 1, y + 1, x, y);
            display(x - 1, y - 1, x, y);
            display(x - 1, y + 1, x, y);
        }
        if (board[i] == 9) {
            gameOver = true;
            return true;
        }
    }
    return false;
}

void reset() {
    for (int i = 0; i < squareNumber; i++) {
        board[i] = 0;
        boardRevealed[i] = 0;
    }
    gameOver = false;
    first_click = true;
    won = false;
    minesLeft = mineNumber;
    unrevealed = squareNumber;
    gameTime = 0;
    player_moves = 0;
    do_the_magic = 0;
    show_magic = 0;
    show_moves = 0;
}

bool show_surroundings(int mouseX, int mouseY) {
    int flag = 0;
    for (int i = -1; i <= 1; i++)
        for (int j = -1; j <= 1; j++) {
            if (mouseX + i >= 0 && mouseX + i < width && mouseY + j >= 0 && mouseY + j < height) {
                if (boardRevealed[(mouseX + i) + width * (mouseY + j)] == 2)
                    flag++;
                if (board[(mouseX + i) + width * (mouseY + j)] == 9 && boardRevealed[(mouseX + i) + width * (mouseY + j)] == 4)
                    flag++;
            }
        }
    if (flag == board[mouseX + width * mouseY]) {
        if (reveal(mouseX, mouseY - 1)      ||
            reveal(mouseX - 1, mouseY - 1)  ||
            reveal(mouseX + 1, mouseY - 1)  ||
            reveal(mouseX - 1, mouseY)      ||
            reveal(mouseX + 1, mouseY)      ||
            reveal(mouseX, mouseY + 1)      ||
            reveal(mouseX - 1, mouseY + 1)  ||
            reveal(mouseX + 1, mouseY + 1))
            return true;
    }
    return false;
}

double abs(double a) {
    if (a < 0)
        return -a;
    else
        return a;
}

float lerp(float a, float b, float c) {
    return a + c * (b - a);
}

int main() {
    srand(time(0)); // the random function using the time in seconds since the Unix epoch as a new seed
    sf::Clock clock, clock2; // used to know the duration of the game
    sf::Time timer, timer2; // used to know the duration of the game

    // Create the window of the application //
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::RenderWindow appWindow(sf::VideoMode((width + gameBorder_right + gameBorder_left) * squareSize, (height + gameBorder_top + gameBorder_bottom) * squareSize, desktop.bitsPerPixel), "Minesweeper!", sf::Style::Resize);

    // Set the Icon
    sf::Image icon;
    if (!icon.loadFromFile("images/icon.png")) {
        std::cout << "can't find the game icon :(" << std::endl;
    }

    // Load the textures used in the game //
    sf::Texture image_sprite, Background_image, faces, logo, optionbuttons, tutorial;
    if (!image_sprite.loadFromFile("images/tiles.png") ||
        !faces.loadFromFile("images/faces.png") ||
        !tutorial.loadFromFile("images/tutorial.png") ||
        !logo.loadFromFile("images/logo.png") ||
        !optionbuttons.loadFromFile("images/optionbuttons.png") ||
        !Background_image.loadFromFile("images/tiles.png", sf::IntRect(16 * squareSize, 0, squareSize, squareSize))) {
        std::cout << "can't find all images :(" << std::endl;
    }

    // Insert texture in sprite //
    sf::Sprite s(image_sprite), background_sprite(Background_image), f(faces), front_logo(logo), optionbuttonsmenu(optionbuttons), tutorial_image(tutorial);
    Background_image.setRepeated(true);
    background_sprite.setTextureRect(sf::IntRect(0, 0, desktop.width, desktop.height));
    front_logo.setOrigin(sf::Vector2f(70, 34));

    front_logo.setPosition(appWindow.getDefaultView().getSize().x / 2, squareSize * 2 - 10);

    // Load the text font //
    sf::Font font;
    if (!font.loadFromFile("fonts/visitor1.ttf"))
        std::cout << "can't find the font (visitor1.ttf) :(" << std::endl;

    // Initialize the game text //
    sf::String playerstring[4];
    sf::Text scoreText, gameTimer, game_version, player_movesText, magic_Text, menu[4], playerinput[4];

    scoreText.setFont(font);
    scoreText.setColor(sf::Color::Red);
    scoreText.setCharacterSize(20);
    scoreText.setStyle(sf::Text::Bold);
    scoreText.setScale(2.f, 2.f);

    gameTimer.setFont(font);
    gameTimer.setColor(sf::Color::Red);
    gameTimer.setCharacterSize(20);
    gameTimer.setStyle(sf::Text::Bold);
    gameTimer.setScale(2.f, 2.f);

    player_movesText.setFont(font);
    player_movesText.setColor(sf::Color::Green);
    player_movesText.setCharacterSize(20);
    player_movesText.setStyle(sf::Text::Bold);
    player_movesText.setScale(2.f, 2.f);

    magic_Text.setFont(font);
    magic_Text.setColor(golden_compass);
    magic_Text.setCharacterSize(20);
    magic_Text.setStyle(sf::Text::Bold);
    magic_Text.setScale(2.f, 2.f);

    // Main menu //

    menu[1].setFont(font);
    menu[1].setString("     Play");
    menu[1].setPosition(appWindow.getDefaultView().getSize().x / 2 - squareSize - 3, 4 * squareSize - 6);

    menu[2].setFont(font);
    menu[2].setString("      Exit");
    menu[2].setPosition(appWindow.getDefaultView().getSize().x / 2 - 2 * squareSize + 8, 6 * squareSize - 6);

        // Main loop //
    while (appWindow.isOpen()) {
        sf::Vector2i pos = sf::Mouse::getPosition(appWindow);
        int mouseX = pos.x / squareSize - gameBorder_left;
        int mouseY = pos.y / squareSize - gameBorder_top;

        switch (menustage) {
        case 0:
            sf::Event main_menu;
            if (appWindow.isOpen()) {
                if (display_size) {
                    width = 10;
                    height = 5;
                    display_size = 0;
                    appWindow.create(sf::VideoMode((width + gameBorder_right + gameBorder_left) * squareSize, (height + gameBorder_top + gameBorder_bottom) * squareSize, desktop.bitsPerPixel), "Minesweeper!");
                    appWindow.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
                }

                // Draw the background //
                appWindow.draw(background_sprite);

                // Draw the Logo image //
                timer2 = clock2.restart();
                logo_animation += timer2.asSeconds();
                front_logo.setScale(lerp(0.98, 1.05, abs(cos(logo_animation))), lerp(0.98, 1.05, abs(cos(logo_animation))));
                appWindow.draw(front_logo);

                // Draw the text background //
                for (int i = 3 * width; i < 6 * width; i++) {
                    s.setTextureRect(sf::IntRect(17 * squareSize, 0, squareSize, squareSize));
                    s.setPosition((i % width + gameBorder_left) * squareSize, (i / width + 1) * squareSize);
                    appWindow.draw(s);
                    if (i == 4 * width - 1)
                        i = 5 * width - 1;
                    if (i == 6 * width - 1)
                        i = 7 * width - 1;
                }

                // Draw the text + Hover //
                if (mouseY == 0)
                    menu[1].setColor(sf::Color::Red);
                else if (mouseY == 2)
                    menu[2].setColor(sf::Color::Red);
                else if (mouseY == 4)
                    menu[3].setColor(sf::Color::Red);
                else {
                    menu[1].setColor(sf::Color::White);
                    menu[2].setColor(sf::Color::White);
                    menu[3].setColor(sf::Color::White);
                }

                // Draw the text menu //
                for (int i = 1; i < 4; i++)
                    appWindow.draw(menu[i]);
            }
            while (appWindow.pollEvent(main_menu)) {
                switch (main_menu.type) {
                case sf::Event::MouseButtonPressed:
                    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
                        mouse_button = 1;
                case sf::Event::MouseButtonReleased:
                    if (mouse_button) {
                        mouse_button = 0;
                        if (mouseY == 0) {
                            menustage = 3;
                            display_size = true;

                        } else if (mouseY == 2) {
                            menustage = 2;
                            display_size = true;

                        } else if (mouseY == 2)
                            appWindow.close();
                    }
                    break;
                default:
                    break;
                }
            }
            appWindow.display();
            break;

        case 1:
            sf::Event play;
            if (appWindow.isOpen()) {
                if (display_size) {
                    minesLeft = mineNumber;
                    squareNumber = width * height;
                    unrevealed = squareNumber;
                    appWindow.create(sf::VideoMode((width + gameBorder_right + gameBorder_left) * squareSize, (height + gameBorder_top + gameBorder_bottom) * squareSize, desktop.bitsPerPixel), "Minesweeper!");
                    appWindow.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
                    display_size = 0;
                }

                // Draw the background blocks //
                appWindow.draw(background_sprite);

                // Main Menu Game Button //
                if (mouseY == -gameBorder_top && mouseX == -gameBorder_left)
                    optionbuttonsmenu.setTextureRect(sf::IntRect(160 * 5 + 64 * 4 + 32, 0, squareSize, squareSize));
                else
                    optionbuttonsmenu.setTextureRect(sf::IntRect(160 * 5 + 64 * 4, 0, squareSize, squareSize));
                optionbuttonsmenu.setPosition(0, 0);
                appWindow.draw(optionbuttonsmenu);

                // Exit Game Button //
                if (mouseY == -gameBorder_top && mouseX == width)
                    optionbuttonsmenu.setTextureRect(sf::IntRect(160 * 5 + 64 * 4 + 32 * 3, 0, squareSize, squareSize));
                else
                    optionbuttonsmenu.setTextureRect(sf::IntRect(160 * 5 + 64 * 4 + 32 * 2, 0, squareSize, squareSize));
                optionbuttonsmenu.setPosition(appWindow.getDefaultView().getSize().x - squareSize, 0);
                appWindow.draw(optionbuttonsmenu);

                // Draw the background for the score table //
                if (width >= 9) {
                    for (int i = width; i < width * 2; i++) {
                        s.setTextureRect(sf::IntRect(17 * squareSize, 0, squareSize, squareSize));
                        s.setPosition((i % width + gameBorder_left) * squareSize, (i / width + 1) * squareSize);
                        appWindow.draw(s);
                        if (i == width + 2)
                            i = width * 2 - 4;
                    }
                } else {
                    for (int i = width; i < width * 2; i++) {
                        s.setTextureRect(sf::IntRect(17 * squareSize, 0, squareSize, squareSize));
                        s.setPosition((i % width + gameBorder_left) * squareSize, (i / width + 1) * squareSize);
                        appWindow.draw(s);
                        if (i == width + 1)
                            i = width * 2 - 3;
                    }
                }

                // Update game flag number//
                std::ostringstream flagleft, magic_left;
                flagleft << minesLeft;
                magic_left << do_the_magic;
                scoreText.setString(flagleft.str());
                magic_Text.setString(magic_left.str());

                if (minesLeft >= 100) {
                    scoreText.setPosition((squareSize + 16), (gameBorder_top - 3) * squareSize + 17.6);
                    magic_Text.setPosition((squareSize + 16), (gameBorder_top - 3) * squareSize + 17.6);
                } else if (minesLeft < 100 && minesLeft >= 10) {
                    scoreText.setPosition((squareSize + 16), (gameBorder_top - 3) * squareSize + 17.6);
                    magic_Text.setPosition((squareSize + 16), (gameBorder_top - 3) * squareSize + 17.6);
                } else {
                    scoreText.setPosition((squareSize + 16), (gameBorder_top - 3) * squareSize + 17.6);
                    magic_Text.setPosition((squareSize + 16), (gameBorder_top - 3) * squareSize + 17.6);
                }

                // Game timer update //
                if (first_click == false && gameOver != 1 && unrevealed != mineNumber) {
                    timer = clock.getElapsedTime();
                    if (timer.asSeconds() > gameTime)
                        if ((width < 9 && gameTime < 99) || (width >= 9 && gameTime < 999))
                            gameTime++;
                }

                // Game timer text + player move text //
                std::ostringstream timespend, moves;
                moves << player_moves;
                timespend << gameTime;
                player_movesText.setString(moves.str());
                gameTimer.setString(timespend.str());

                if (gameTime >= 100 || player_moves >= 100) {
                    gameTimer.setPosition((width - gameBorder_right * 1.5) * squareSize, squareSize + 17.6);
                    player_movesText.setPosition((width - gameBorder_right * 1.5) * squareSize, squareSize + 17.6);
                } else if (gameTime < 100 && gameTime >= 10 || player_moves < 100 && player_moves >= 10) {
                    gameTimer.setPosition((width - gameBorder_right / 1.5) * squareSize, squareSize + 17.6);
                    player_movesText.setPosition((width - gameBorder_right / 1.5) * squareSize, squareSize + 17.6);
                } else {
                    gameTimer.setPosition((width - gameBorder_right / 1.5) * squareSize, squareSize + 17.6);
                    player_movesText.setPosition((width - gameBorder_right / 1.5) * squareSize, squareSize + 17.6);
                }

                // Face sprite control //
                // Set the face texture sprite depending on width //
                if (width % 2 == 0)
                    faceSize = squareSize * 2;
                else
                    faceSize = squareSize;

                // Set the face position //
                f.setPosition(appWindow.getDefaultView().getSize().x / 2 - faceSize / 2, squareSize);

                unsigned int on_face = appWindow.getDefaultView().getSize().x / 2 - faceSize / 2;
                if (sf::Mouse::isButtonPressed(sf::Mouse::Left) ||
                    sf::Mouse::isButtonPressed(sf::Mouse::Middle) ||
                    sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
                    if (pos.x >= on_face && pos.x <= on_face + faceSize && mouseY == -3) {
                        if (width % 2 == 0)
                            f.setTextureRect(sf::IntRect(faceSize * 1 + squareSize * 7, 0, faceSize, squareSize));
                        else
                            f.setTextureRect(sf::IntRect(faceSize * 1, 0, faceSize, squareSize));
                        if (gameOver == 1 || unrevealed == mineNumber) {

                        }
                        reset();
                    } else if (mouseX >= 0 && mouseX < width && mouseY >= 0 && mouseY < height) {
                        if (width % 2 == 0)
                            f.setTextureRect(sf::IntRect(faceSize * 3 + squareSize * 7, 0, faceSize, squareSize));
                        else
                            f.setTextureRect(sf::IntRect(faceSize * 3, 0, faceSize, squareSize));
                    }
                } else if (sf::Mouse::isButtonPressed(sf::Mouse::Right) ||
                    sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
                    if (pos.x >= on_face && pos.x <= on_face + faceSize && mouseY == -3) {
                        if (width % 2 == 0)
                            f.setTextureRect(sf::IntRect(faceSize * 6 + squareSize * 7, 0, faceSize, squareSize));
                        else
                            f.setTextureRect(sf::IntRect(faceSize * 6, 0, faceSize, squareSize));
                    }
                } else {
                    if (width % 2 == 0)
                        f.setTextureRect(sf::IntRect(faceSize * 0 + squareSize * 7, 0, faceSize, squareSize));
                    else
                        f.setTextureRect(sf::IntRect(faceSize * 0, 0, faceSize, squareSize));
                }

                // Draw the game board //
                for (int i = 0; i < squareNumber; i++) {
                    if (boardRevealed[i] == 1 && board[i] == -1) // reveal the death position //
                        s.setTextureRect(sf::IntRect(10 * squareSize, 0, squareSize, squareSize));
                    else if (boardRevealed[i] == 1) // reveal the i square //
                        s.setTextureRect(sf::IntRect(board[i] * squareSize, 0, squareSize, squareSize));
                    else if (boardRevealed[i] == 0) // place the hide square //
                        s.setTextureRect(sf::IntRect(11 * squareSize, 0, squareSize, squareSize));
                    else if (boardRevealed[i] == 2) // place the flag square //
                        s.setTextureRect(sf::IntRect(12 * squareSize, 0, squareSize, squareSize));
                    else if (boardRevealed[i] == 3) // place the not for sure flag //
                        s.setTextureRect(sf::IntRect(13 * squareSize, 0, squareSize, squareSize));
                    else if (boardRevealed[i] == 4) // look behind the block, special magic //
                        s.setTextureRect(sf::IntRect(board[i] * squareSize, squareSize, squareSize, squareSize));

                    // Game End //
                    // if the player won the game, places the flag on every bomb that wasn't marked //
                    if (board[i] == 9 && unrevealed == mineNumber && won && boardRevealed[i] != 2 && boardRevealed[i] != 4)
                        s.setTextureRect(sf::IntRect(12 * squareSize, 0, squareSize, squareSize));
                    // if the player lose the game, places the flag on every bomb that was marked //
                    else if (board[i] == 9 && gameOver && boardRevealed[i] == 2)
                        s.setTextureRect(sf::IntRect(12 * squareSize, 0, squareSize, squareSize));
                    // if the player lose the game, replaces the flag on every bomb that was marked wrong //
                    else if (board[i] != 9 && gameOver && boardRevealed[i] == 2)
                        s.setTextureRect(sf::IntRect(15 * squareSize, 0, squareSize, squareSize));
                    // in another case shows all the bombs //
                    else if (board[i] == 9 && gameOver && boardRevealed[i] != 4)
                        s.setTextureRect(sf::IntRect(9 * squareSize, 0, squareSize, squareSize));

                    s.setPosition((i % width + gameBorder_left) * squareSize, (i / width + gameBorder_top) * squareSize);
                    appWindow.draw(s);
                }

                //focus on the block where the player hold the left mouse press
                if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && (boardRevealed[mouseX + width * mouseY] == 0 || boardRevealed[mouseX + width * mouseY] == 3)) {
                    if ((mouseX >= 0) && (mouseX < width) && (mouseY >= 0) && (mouseY < height)) {
                        if (boardRevealed[mouseX + width * mouseY] == 3)
                            s.setTextureRect(sf::IntRect(14 * squareSize, 0, squareSize, squareSize));
                        else
                            s.setTextureRect(sf::IntRect(0 * squareSize, 0, squareSize, squareSize));
                        s.setPosition((gameBorder_left + mouseX) * squareSize, (gameBorder_top + mouseY) * squareSize);
                        if (gameOver != 1 && unrevealed != mineNumber)
                            appWindow.draw(s);
                    }
                }

                //focus on the block where the player hold the  middle-click or LShift
                else if (sf::Mouse::isButtonPressed(sf::Mouse::Middle) || sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
                    for (int i = -1; i <= 1; i++)
                        for (int j = -1; j <= 1; j++)
                            if (boardRevealed[(mouseX + i) + width * (mouseY + j)] == 0 || boardRevealed[(mouseX + i) + width * (mouseY + j)] == 3) {
                                if (mouseX + i >= 0 && mouseX + i < width && mouseY + j >= 0 && mouseY + j < height) {
                                    if (boardRevealed[(mouseX + i) + width * (mouseY + j)] == 3)
                                        s.setTextureRect(sf::IntRect(14 * squareSize, 0, squareSize, squareSize));
                                    else
                                        s.setTextureRect(sf::IntRect(0 * squareSize, 0, squareSize, squareSize));
                                    s.setPosition((gameBorder_left + mouseX + i) * squareSize, (gameBorder_top + mouseY + j) * squareSize);
                                    if (gameOver != 1 && unrevealed != mineNumber)
                                        appWindow.draw(s);
                                }
                            }
                }

                // If game win //
                if (unrevealed == mineNumber && !gameOver) {
                    if (width % 2 == 0)
                        f.setTextureRect(sf::IntRect(faceSize * 4 + squareSize * 7, 0, faceSize, squareSize));
                    else
                        f.setTextureRect(sf::IntRect(faceSize * 4, 0, faceSize, squareSize));
                    appWindow.draw(f);
                    if (won == false) {
                        won = true;

                        minesLeft = 0;
                    }
                }

                // If game lose //
                if (gameOver == 1) {
                    if (width % 2 == 0)
                        f.setTextureRect(sf::IntRect(faceSize * 5 + squareSize * 7, 0, faceSize, squareSize));
                    else
                        f.setTextureRect(sf::IntRect(faceSize * 5, 0, faceSize, squareSize));
                    appWindow.draw(f);
                }

                appWindow.draw(f);
                if (show_magic && magic_option)
                    appWindow.draw(magic_Text);
                else appWindow.draw(scoreText);
                if (show_moves)
                    appWindow.draw(player_movesText);
                else appWindow.draw(gameTimer);;
            }
            while (appWindow.pollEvent(play)) {
                if (play.type == sf::Event::Closed)
                    appWindow.close();
                if ((gameOver != 1) && (unrevealed != mineNumber) && (mouseX >= 0) && (mouseX < width) && (mouseY >= 0) && (mouseY < height)) {
                    switch (play.type) {
                    case sf::Event::MouseButtonPressed:
                        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
                            mouse_button = 1;
                        else if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
                            mouse_button = 2;
                        else if (sf::Mouse::isButtonPressed(sf::Mouse::Middle))
                            mouse_button = 3;
                        break;
                    case sf::Event::KeyPressed:
                        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
                            mouse_button = 2;
                        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                            mouse_button = 3;
                        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                            mouse_button = 4;
                        if (sf::Keyboard::isKeyPressed(sf::Keyboard::F2)) {
                            if (gameOver == 1 || unrevealed == mineNumber) {
                            }
                            reset();
                        }
                        break;
                    case sf::Event::KeyReleased:
                        if (mouse_button == 2) {
                            if (boardRevealed[mouseY * width + mouseX] == 2) {
                                minesLeft++;
                                if (mark_flag)
                                    boardRevealed[mouseY * width + mouseX] = 3;
                                else
                                    boardRevealed[mouseY * width + mouseX] = 0;
                            } else if (boardRevealed[mouseY * width + mouseX] == 3) {

                                boardRevealed[mouseY * width + mouseX] = 0;
                            } else if (boardRevealed[mouseY * width + mouseX] == 0 && minesLeft > -9) {
                                minesLeft--;
                                boardRevealed[mouseY * width + mouseX] = 2;

                            }
                            mouse_button = 0;
                        } else if (mouse_button == 3) {
                            if (boardRevealed[mouseY * width + mouseX] == 1) {

                            }
                            mouse_button = 0;
                        }
                        if (mouse_button == 4 && do_the_magic != 0) {
                            if (boardRevealed[mouseY * width + mouseX] == 0) {
                                boardRevealed[mouseY * width + mouseX] = 4;
                                if (magic_option)
                                    do_the_magic--;
                                player_moves++;
                                if (board[mouseY * width + mouseX] == 9)
                                    minesLeft--;
                            }
                            mouse_button = 0;
                        }
                        break;
                    case sf::Event::MouseButtonReleased:
                        if (mouse_button == 1) {
                            if (first_click) {
                                // generate the map after the first click every time //
                                first_click = false;
                                clock.restart();

                                // mark the position as a safe point //
                                board[mouseY * width + mouseX] = -2;

                                // first things first (place the mines) //
                                placeMines(mouseY * width + mouseX);

                                // place the numbers //
                                for (int i = 0; i < squareNumber; i++)
                                    countSurrounding(i % width, i / width);
                                if (magic_option)
                                    do_the_magic = 1;
                            }
                            if (boardRevealed[mouseY * width + mouseX] == 0 || boardRevealed[mouseY * width + mouseX] == 3) {
                                if (reveal(mouseX, mouseY))
                                    board[mouseY * width + mouseX] = -1;
                            }
                            mouse_button = 0;
                        } else if (mouse_button == 2) {
                            if (boardRevealed[mouseY * width + mouseX] == 2) {
                                minesLeft++;

                                if (mark_flag)
                                    boardRevealed[mouseY * width + mouseX] = 3;
                                else
                                    boardRevealed[mouseY * width + mouseX] = 0;
                            } else if (boardRevealed[mouseY * width + mouseX] == 3) {

                                boardRevealed[mouseY * width + mouseX] = 0;
                            } else if (boardRevealed[mouseY * width + mouseX] == 0 && minesLeft > -9) {
                                minesLeft--;
                                boardRevealed[mouseY * width + mouseX] = 2;

                            }
                            mouse_button = 0;
                        } else if (mouse_button == 3) {
                            mouse_button = 0;
                        }
                        break;
                    default:
                        break;
                    }
                } else {
                    switch (play.type) {
                    case sf::Event::MouseButtonPressed:
                        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
                            mouse_button = 1;
                    case sf::Event::MouseButtonReleased:
                        if (mouse_button == 1) {
                            if (mouseY == -(gameBorder_top - 2) && mouseX <= width - gameBorder_right && mouseX >= 0) {
                                if (width < 9) {
                                    if (mouseX >= width - gameBorder_right - 1) {
                                        mouse_button = 0;
                                        if (show_moves)
                                            show_moves = false;
                                        else
                                            show_moves = true;
                                    }
                                    if (mouseX <= 1) {
                                        mouse_button = 0;
                                        if (show_magic)
                                            show_magic = false;
                                        else
                                            show_magic = true;
                                    }
                                } else if (mouseX <= 2) {
                                    mouse_button = 0;
                                    if (show_magic)
                                        show_magic = false;
                                    else
                                        show_magic = true;
                                } else if (mouseX >= width - gameBorder_right - 2) {
                                    mouse_button = 0;
                                    if (show_moves)
                                        show_moves = false;
                                    else
                                        show_moves = true;
                                }
                            }
                            if (mouseY == -gameBorder_top && mouseX == -gameBorder_left) {
                                mouse_button = 0;
                                menustage = 0;
                                display_size = 1;
                                reset();
                            }
                            if (mouseY == -gameBorder_top && mouseX == width) {
                                mouse_button = 0;
                                appWindow.close();
                            }
                        }
                        break;
                    case sf::Event::KeyPressed:
                        if (sf::Keyboard::isKeyPressed(sf::Keyboard::F2)) {
                            reset();
                        }
                        break;
                    default:
                        break;
                    }
                }
            }
            appWindow.display();
            break;

        case 3:
            sf::Event playmode;
            if (appWindow.isOpen()) {
                if (display_size) {
                    width = 5;
                    height = 6;
                    appWindow.create(sf::VideoMode((width + gameBorder_right + gameBorder_left) * squareSize, (height + gameBorder_top + gameBorder_bottom) * squareSize, desktop.bitsPerPixel), "Minesweeper!");
                    appWindow.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
                    display_size = 0;
                }

                // Draw background //
                appWindow.draw(background_sprite);

                // Main Menu Game Button //
                if (mouseY == -gameBorder_top && mouseX == -gameBorder_left)
                    optionbuttonsmenu.setTextureRect(sf::IntRect(160 * 5 + 64 * 4 + 32, 0, squareSize, squareSize));
                else
                    optionbuttonsmenu.setTextureRect(sf::IntRect(160 * 5 + 64 * 4, 0, squareSize, squareSize));
                optionbuttonsmenu.setPosition(0, 0);
                appWindow.draw(optionbuttonsmenu);

                // Exit Game Button //
                if (mouseY == -gameBorder_top && mouseX == width)
                    optionbuttonsmenu.setTextureRect(sf::IntRect(160 * 5 + 64 * 4 + 32 * 3, 0, squareSize, squareSize));
                else
                    optionbuttonsmenu.setTextureRect(sf::IntRect(160 * 5 + 64 * 4 + 32 * 2, 0, squareSize, squareSize));
                optionbuttonsmenu.setPosition(appWindow.getDefaultView().getSize().x - squareSize, 0);
                appWindow.draw(optionbuttonsmenu);

                // Draw the choices //
                for (unsigned int i = 0; i <= 5; i++) {
                    if (i == 0) {
                        // Draw Title: Options //
                        optionbuttonsmenu.setTextureRect(sf::IntRect(0, squareSize, 160, squareSize));
                        optionbuttonsmenu.setPosition(gameBorder_left * squareSize, squareSize);
                        appWindow.draw(optionbuttonsmenu);
                    } else {
                        // Draw Game Mode: Begginer, Intermediate, Expert, Custom //
                        optionbuttonsmenu.setTextureRect(sf::IntRect(160 * i, squareSize, 160, squareSize));
                        optionbuttonsmenu.setPosition(gameBorder_left * squareSize, squareSize * (i + 2));
                        appWindow.draw(optionbuttonsmenu);
                        i++;
                    }
                }

                // Draw Game Mode: Begginer, Intermediate, Expert, Custom  Hover //
                if ((mouseY == -1 || mouseY == 1 || mouseY == 3 ) && mouseX >= 0 && mouseX <= 4) {
                    optionbuttonsmenu.setTextureRect(sf::IntRect(160 * (4 + mouseY - 1), squareSize, 160, squareSize));
                    optionbuttonsmenu.setPosition(gameBorder_left * squareSize, squareSize * (4 + mouseY));
                    appWindow.draw(optionbuttonsmenu);
                }
            }
            while (appWindow.pollEvent(playmode)) {
                switch (playmode.type) {
                case sf::Event::MouseButtonPressed:
                    if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
                        mouse_button = 1;
                    break;
                case sf::Event::MouseButtonReleased:
                    if (mouse_button) {
                        mouse_button = 0;
                        if (mouseY == -1 && mouseX >= 0 && mouseX <= 4) {
                            width = 9;
                            height = 9;
                            mineNumber = 10;
                            display_size = true;
                            menustage = 1;
                        }
                        if (mouseY == 1 && mouseX >= 0 && mouseX <= 4) {
                            width = 16;
                            height = 16;
                            mineNumber = 40;
                            display_size = true;
                            menustage = 1;

                        }
                        if (mouseY == 3 && mouseX >= 0 && mouseX <= 4) {
                            width = 30;
                            height = 16;
                            mineNumber = 99;
                            display_size = true;
                            menustage = 1;

                        }
                        if (mouseY == 5 && mouseX >= 0 && mouseX <= 4) {
                            menustage = 4;
                            display_size = true;

                        }
                        if (mouseY == -gameBorder_top && mouseX == -gameBorder_left) {
                            menustage = 0;
                            display_size = 1;
                        }
                        if (mouseY == -gameBorder_top && mouseX == width)
                            appWindow.close();
                    }
                    break;
                }
            }
            appWindow.display();
            break;

        default:
            appWindow.close();
            break;
        }
    }
    return 0;
}
