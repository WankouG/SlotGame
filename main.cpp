#include <iostream>
#include <limits>
#include <cstdlib> // for std::rand
#include <ctime> // for std::time

#ifdef _WIN32
#include <conio.h> // for _getch() in windows
#else
#include <ncurses.h> // for _getch() in Linux
#endif

struct symbol {
    const char* name;
    int multiplier;
};

symbol symbols[] = {
    {"Cherry", 1},
    {"Lemon", 2},
    {"Orange", 2},
    {"Plum", 4},
    {"Bell", 5},
    {"Bar", 10}
};

//enum群
enum class GameState {
    START,
    BET,
    PLAY,
    PLAY_WAIT,
    EXIT,
};

struct GameData {
    int credit = 100;
    int bet = 0;
    int choice = 0;
    int dividend = 0;
    GameState gamestate = GameState::START; //ユーザーの選択を格納する変数。ゲームステート型
    symbol* reel_symbols[3]; //リールのシンボルを格納する配列。
};

//関数プロトタイプ宣言
void init();
int get_bet(int credit);
void spin_reels(GameData &gamedata);
int slot_lottery(const GameData &gamedata);
void render_menu(const GameData &gamedata);
int get_key();
void get_clear_cli();

int main() {
    //変数宣言と初期化処理
    #ifndef _WIN32
        init();       
    #endif

    std::srand(std::time(0)); // 乱数のシードを現在の時刻で初期化
    GameData gamedata;
    
    //ゲームループ作成
    while(true){
        render_menu(gamedata);
        switch(gamedata.gamestate){
            case GameState::START:{
                    // 上下キーの入力を格納する変数
                #ifdef _WIN32
                    int key = get_key();
                    if (key == 0 || key == 0xE0) {  
                                /* 上ボタンが押された場合の処理.
                                0xE0は拡張キーのプレフィックスコードです。0xは以下のコードが16進数であることを示す接頭辞です。
                                拡張キーは、通常のキーとは異なる特別なキーであり、矢印キーやファンクションキーなどが含まれます。
                                _getch()関数は、拡張キーを押した場合、最初に0または0xE0を返し、次に実際のキーコードを返します。*/     
                                key = get_key();
                                if (key == 72) { // 上ボタンが押された場合の処理
                                    gamedata.choice = (gamedata.choice - 1 + 2) % 2; // 上ボタンが押された場合、選択肢を上に移動させる。選択肢は0と1の2つなので、2で割った余りを取ることで循環させる。
                                }
                                else if (key == 80) { // 下ボタンが押された場合の処理
                                    gamedata.choice = (gamedata.choice + 1 ) % 2; // 下ボタンが押された場合、選択肢を下に移動させる。選択肢は0と1の2つなので、2で割った余りを取ることで循環させる。
                                }
                            }
                    else if (key == 13) { // 決定ボタンが押された場合
                                gamedata.gamestate = gamedata.choice == 0 ? GameState::BET : GameState::EXIT; // 選択肢に応じてGameStateを返す
                            }
                    else {
                        continue;
                        }
                #else
                    int key = get_key();
                    if(key == KEY_UP){
                        choice = (choice - 1 + 2) % 2;
                    }
                    else if(key == KEY_DOWN){
                        choice = (choice + 1) % 2;
                    }

                    if(key == '\n'){
                        return choice == 0 ? GameState::BET : GameState::EXIT;
                    }
                    for (int i = 0; i < 2; i++) {
                            if (i == choice) {
                                    std::cout << "\033[33m" << (i == 0 ? "Start" : "Exit") << "\033[0m" << std::endl; // 選択中の項目を黄色で表示するANSIエスケープシーケンス
                            }
                            else {
                                    std::cout << (i == 0 ? "Start" : "Exit") << std::endl;
                            }
                        }
                #endif // ユーザーの選択を取得する関数を呼び出す
                break;
            }
            case GameState::BET:{
                gamedata.bet = get_bet(gamedata.credit); // ユーザーにベット額を入力させる関数を呼び出す
                gamedata.gamestate = GameState::PLAY;
                break;
            }
            case GameState::PLAY: {
                gamedata.credit -= gamedata.bet;
                //リールの表示。
                //シンボルリストに乱数を使って、リールのシンボルを決定する。
                spin_reels(gamedata);

                //抽選処理
                gamedata.dividend = slot_lottery(gamedata);
                gamedata.credit += (gamedata.bet * gamedata.dividend);

                //抽選後、クレジットが０以下になった場合、ゲームを終了する。
  

                gamedata.gamestate = GameState::PLAY_WAIT;
                break;
            }
            case GameState::PLAY_WAIT:{
                int input_key = get_key();
                if(input_key == 13){//ENTER
                    gamedata.gamestate = GameState::PLAY;
                    break;
                }
                else if(input_key == 8){//BACK
                    gamedata.gamestate = GameState::START;
                    break;
                }
                else{
                    continue;
                }
            }
            case GameState::EXIT:{
                #ifndef _WIN32
                    endwin();
                #endif
                break;
            }

        }

        if (gamedata.credit <= 0) {
                gamedata.gamestate = GameState::EXIT;
        }

        if(gamedata.gamestate == GameState::EXIT){
            get_clear_cli();
            std::cout << "Thank you for playing! Your final credit is: " << gamedata.credit << std::endl;
            break;
        }
    }
    return 0;
}

void init(){
    #ifndef _WIN32
        initscr();
        cbreak();
        noecho();
        keypad(stdscr,true);
    #endif
}

int get_bet(int credit) {
    // ユーザーに有効なベット額を入力させる
    int bet = 0;
        while (true) {
            //数値以外、マイナス値、クレジット以上の値を入力した場合は再度入力を促す
            //このBOOLは入力の成功を真とする。
            if (std::cin >> bet && bet > 0 && bet <= credit) {
                return bet;
            }

            /*入力が失敗した場合、cinの状態をクリアして再度入力を促す。
            C++では、入力ストリームが失敗した場合、ストリームの状態がfailbitに設定されます。
            これにより、以降の入力操作が無効になります。
            したがって、入力が失敗した場合は、まずストリームの状態をクリアする必要があります。*/

            //Failbit状態の解除
            std::cin.clear();

            //入力バッファのクリア
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
        }
}


void spin_reels(GameData &gamedata) {
     for (int i = 0; i < 3; i++) {
            gamedata.reel_symbols[i] = &symbols[std::rand() % 6];
        }
}

int slot_lottery(const GameData &gamedata) {
    int dividend = 0;
        //あたり抽選。シンボルごとに配当を用意する予定。
        if (gamedata.reel_symbols[0] == gamedata.reel_symbols[1] && gamedata.reel_symbols[1] == gamedata.reel_symbols[2]){
            dividend = gamedata.reel_symbols[0]->multiplier;
        }
        else {
            dividend = 0;
        }

        return dividend;
}

void render_menu(const GameData &gamedata) {
    //描画処理。現在の選択肢を黄色で表示する。選択肢の描画は、コンソールのカーソル位置を制御することで実現できる。
    switch (gamedata.gamestate){
        //各ゲームステートでの描画を設定
        case GameState::START:
            //スタートメニューの描画
            // ユーザーのクレジット残高を表示する
            // ユーザーの選択に応じて処理を分岐する.０はゲーム開始、１は終了、その他の値は無効な選択として再度入力を促す。
            get_clear_cli(); // 画面をクリアしてカーソルを左上に移動するANSIエスケープシーケンス
            std::cout << "Welcome to the Slot Game!" << std::endl;
            std::cout << "You have " << gamedata.credit << " credits." << std::endl;
            std::cout << "Please select an option:" << std::endl;
            std::cout << "------------------------" << std::endl;
            for (int i = 0; i < 2; i++)
            {
                if (i == gamedata.choice){
                    std::cout << "\033[33m" << (i == 0 ? "Start" : "Exit") << "\033[0m" << std::endl;
                }
                else{
                    std::cout << (i == 0 ? "Start" : "Exit") << std::endl;
                }
            }
            // カーソルを2行上に移動するANSIエスケープシーケンス
            std::cout << "\033[0J\033[3F" << std::endl;
            break;

        case GameState::BET:{
            std::cout << "your bet credit: " << gamedata.bet << std::endl;
            std::cout << "\033[2J\033[1;1H";
            std::cout << "Please enter a valid bet amount: ";
            break;
        }
        case GameState::PLAY:{
            break;
        }
        case GameState::PLAY_WAIT:{
            get_clear_cli();  
            std::cout << "you bet " << gamedata.bet << " credits." << "your credit is now " << gamedata.credit << std::endl;
            for(int i = 0; i < 3; i++){
                std::cout << gamedata.reel_symbols[i]->name << " ";
            }
            std::cout << "\n" << std::endl;
     
            if (gamedata.dividend > 0){
                std::cout << "you win" << gamedata.reel_symbols[0]->multiplier * gamedata.bet << " credits paied!" << std::endl;
                std::cout << "\033[33mJACKPOT!!\033[0m" << std::endl;
            }
            else {
                std::cout << "No win this time. Better luck next spin!\n" << std::endl;
            }

            //結果の表示処理
            std::cout << "Press the Enter key to spin the slots again with the current bet amount." << std::endl;
            std::cout << "Press the Back key to return to the initial screen." << std::endl;

            break;
        }
        case GameState::EXIT:{
            std::cout << "Thank you for playing! Your final credit is: " << gamedata.credit << std::endl;
            break;
        }
    }
        
}

int get_key(){
    //入力キーを受け取る。クロスプラットフォーム対応。
    #ifdef _WIN32
        return _getch();
    #else
        return getch();
    #endif
}

void get_clear_cli(){
    //画面消去、カーソルの先頭位置移動を行う関数
    std::cout << "\033[2J\033[3J\033[1;1H" << std::flush;
}