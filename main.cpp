#include <iostream>
#include <limits>
#include <cstdlib> // for std::rand
#include <ctime> // for std::time
#include <conio.h> // for _getch()

struct symbol {
    const char* name;
    float multiplier;
};

int credit  = 100;

symbol symbols[] = {
    {"Cherry", 1.1},
    {"Lemon", 1.5},
    {"Orange", 2.0},
    {"Plum", 4.0},
    {"Bell", 4.5},
    {"Bar", 10.0}
};

//enum群
enum class GameState {
    START,
    BET,
    PLAY,
    EXIT,
};

//関数プロトタイプ宣言
int get_bet();
GameState get_choice();
void spin_reals(symbol* real_symbols[]);
void slot_lottery(symbol* real_symbols[], int bet);
void render_menu(int choice);

int main() {
    //変数宣言と初期化処理
    int bet = 0;
    const char* select_menu[] = {"start", "Exit"}; //セレクトメニューの選択肢を格納する配列。いずれ矢印で選択できるようにする予定。
    GameState choice = GameState::START; //ユーザーの選択を格納する変数。ゲームステート型
    symbol* real_symbols[3]; //リールのシンボルを格納する配列。
    std::srand(std::time(0)); // 乱数のシードを現在の時刻で初期化
    
    //ゲームループ作成
    while(true){
        switch(choice){
            case GameState::START:
                choice = get_choice(); // ユーザーの選択を取得する関数を呼び出す
                // ユーザーのクレジット残高を表示する
                // ユーザーの選択に応じて処理を分岐する.０はゲーム開始、１は終了、その他の値は無効な選択として再度入力を促す。
                if (choice == GameState::START) {
                    choice = GameState::BET;
                    std::cout << "Starting the game..." << std::endl;
                }
                else if (choice == GameState::EXIT) {
                    std::cout << "Thank you for playing! Your final credit is: " << credit << std::endl;
                    break;
                }
            case GameState::BET:
                bet = get_bet(); // ユーザーにベット額を入力させる関数を呼び出す
                //最終ベット数とクレジット残高を表示する    

            case GameState::PLAY: 
                std::cout << "\033[2J\033[3J\033[1;1H" << std::flush;   
                credit -= bet;
                std::cout << "you bet " << bet << " credits." << "your credit is now " << credit << std::endl;
                //リールの表示。
                //シンボルリストに乱数を使って、リールのシンボルを決定する。
                spin_reals(real_symbols);

                //抽選処理
                slot_lottery(real_symbols, bet);

                //結果の表示処理
                std::cout << "Press the Enter key to spin the slots again with the current bet amount." << std::endl;
                std::cout << "Press the Back key to return to the initial screen." << std::endl;
                while(true){
                    int input_key = _getch();
                    if(input_key == 13){
                        choice = GameState::PLAY;
                        break;
                    }
                    else if(input_key == 8){
                        choice = GameState::START;
                        break;
                    }
                    else{
                        continue;
                    }
                }

                //抽選後、クレジットが０以下になった場合、ゲームを終了する。
                if (credit <= 0) {
                    std::cout << "Thank you for playing! Your final credit is: " << credit << std::endl;
                    choice = GameState::EXIT;
                }

            case GameState::EXIT:
                break;
        }
        if (choice == GameState::EXIT){
            break;
        }
    }
    return 0;
}

GameState get_choice() {
    int choice = 0;
    render_menu(choice); // メニューを描画する関数を呼び出す

    while(true) {
    // 上下キーの入力を格納する変数
        int arrow_key = _getch();
        if (arrow_key == 0 || arrow_key == 0xE0) {  
                    /* 上ボタンが押された場合の処理.
                    0xE0は拡張キーのプレフィックスコードです。0xは以下のコードが16進数であることを示す接頭辞です。
                    拡張キーは、通常のキーとは異なる特別なキーであり、矢印キーやファンクションキーなどが含まれます。
                    _getch()関数は、拡張キーを押した場合、最初に0または0xE0を返し、次に実際のキーコードを返します。*/     
                    arrow_key = _getch();
                    render_menu(choice); // メニューを描画する関数を呼び出す
                    if (arrow_key == 72) { // 上ボタンが押された場合の処理
                        choice = (choice - 1 + 2) % 2; // 上ボタンが押された場合、選択肢を上に移動させる。選択肢は0と1の2つなので、2で割った余りを取ることで循環させる。
                    }
                    else if (arrow_key == 80) { // 下ボタンが押された場合の処理
                        choice = (choice + 1 ) % 2; // 下ボタンが押された場合、選択肢を下に移動させる。選択肢は0と1の2つなので、2で割った余りを取ることで循環させる。
                    }
                }
        else if (arrow_key == 13) { // 決定ボタンが押された場合
                    return choice == 0 ? GameState::PLAY : GameState::EXIT; // 選択肢に応じてGameStateを返す
                }
        else {
            continue;
            }

        for (int i = 0; i < 2; i++) {
                    if (i == choice) {
                        std::cout << "\033[33m" << (i == 0 ? "Start" : "Exit") << "\033[0m" << std::endl; // 選択中の項目を黄色で表示するANSIエスケープシーケンス
                    }
                    else {
                        std::cout << (i == 0 ? "Start" : "Exit") << std::endl;
                    }
                }
    }
}

int get_bet() {
    // ユーザーに有効なベット額を入力させる
    int bet = 0;

        std::cout << "your bet credit: " << bet << std::endl;
        while (true) {
            std::cout << "\033[2J\033[1;1H";
            //数値以外、マイナス値、クレジット以上の値を入力した場合は再度入力を促す
            std::cout << "Please enter a valid bet amount: ";
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


void spin_reals(symbol* real_symbols[]) {
     for (int i = 0; i < 3; i++) {
            real_symbols[i] = &symbols[std::rand() % 6];
            std::cout << real_symbols[i]->name << " ";
        }
        //改行
        std::cout << "\n" << std::endl;
}

void slot_lottery(symbol* real_symbols[], int bet) {
        //あたり抽選。シンボルごとに配当を用意する予定。
        if (real_symbols[0] == real_symbols[1] && real_symbols[1] == real_symbols[2]){
            std::cout << "\e[33mJACKPOT!!\e[0m" << std::endl;
            credit += real_symbols[0]->multiplier * bet;
            std::cout << "you win" << real_symbols[0]->multiplier * bet << " credits paied!" << std::endl;
        }
        else {
            std::cout << "No win this time. Better luck next spin!\n" << std::endl;
        }
}

void render_menu(int choice) {
        //描画処理。現在の選択肢を黄色で表示する。選択肢の描画は、コンソールのカーソル位置を制御することで実現できる。
        std::cout << "\033[2J\033[3J\033[1;1H" << std::flush; // 画面をクリアしてカーソルを左上に移動するANSIエスケープシーケンス
        std::cout << "Welcome to the Slot Game!" << std::endl;
        std::cout << "You have " << credit << " credits." << std::endl;
        std::cout << "Please select an option:" << std::endl;
        std::cout << "------------------------" << std::endl;
        for (int i = 0; i < 2; i++)
        {
            if (i == choice){
                std::cout << "\033[33m" << (i == 0 ? "Start" : "Exit") << "\033[0m" << std::endl;
            }
            else{
                std::cout << (i == 0 ? "Start" : "Exit") << std::endl;
            }
        }
        
        std::cout << "\033[0J\033[3F" << std::endl;// カーソルを2行上に移動するANSIエスケープシーケンス
        
}