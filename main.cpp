#include <iostream>
#include <limits>
#include <cstdlib> // for std::rand
#include <ctime> // for std::time

struct symbol {
    const char* name;
    int payout;
};

int credit  = 100;
symbol symbols[] = {
    {"Cherry", 10},
    {"Lemon", 20},
    {"Orange", 30},
    {"Plum", 40},
    {"Bell", 50},
    {"Bar", 100}
};


int main() {
    //変数宣言と初期化処理
    int bet = 0;
    const char* select_menu[] = {"start", "Exit"}; //セレクトメニューの選択肢を格納する配列。いずれ矢印で選択できるようにする予定。
    int choice = 0; //ユーザーの選択を格納する変数。
    symbol* real_symbols[3]; //リールのシンボルを格納する配列。
    std::srand(std::time(0)); // 乱数のシードを現在の時刻で初期化
    

    std::cout << "Welcome to the Slot Game!" << std::endl;
    //ゲームループ作成
    while(true){
        // クレジット処理
        std::cout << "You have " << credit << " credits." << std::endl;

        //ゲームを継続するか、終了するかをユーザーに尋ねる
        std::cout << select_menu[0] << " or " << select_menu[1] << "? (Enter 0 for start, 1 for exit): ";
        std::cin >> choice;
        // ユーザーの選択に応じて処理を分岐する.０はゲーム開始、１は終了、その他の値は無効な選択として再度入力を促す。
        if (choice == 1) {
            std::cout << "Thank you for playing! Your final credit is: " << credit << std::endl;
            break;
        }
        else if (choice != 0) {
            std::cout << "Invalid choice. Please enter 0 to start or 1 to exit." << std::endl;
            continue;
        }

        // ユーザーに有効なベット額を入力させる
        std::cout << "your bet credit: " << std::endl;
        while (true) {
            //数値以外、マイナス値、クレジット以上の値を入力した場合は再度入力を促す
            std::cout << "Please enter a valid bet amount: ";
            //このBOOLは入力の成功を真とする。
            if (std::cin >> bet && bet > 0 && bet <= credit) {
                break;
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

        //最終ベット数とクレジット残高を表示する
        credit -= bet;
        std::cout << "you bet " << bet << " credits." << "your credit is now " 
                << credit << std::endl;

        //リールの表示。printで１列表示。
        //シンボルリストに乱数を使って、リールのシンボルを決定する。
        for (int i = 0; i < 3; i++) {
            real_symbols[i] = &symbols[std::rand() % 6];
            std::cout << real_symbols[i]->name << " ";
        }
        //改行
        std::cout << "\n" << std::endl;


        //あたり抽選。シンボルごとに配当を用意する予定。
        if (real_symbols[0] == real_symbols[1] && real_symbols[1] == real_symbols[2]){
            std::cout << "JACKPOT!" << std::endl;
            credit += real_symbols[0]->payout * bet;
            std::cout << "you win" << real_symbols[0]->payout * bet << " credits paied!" << std::endl;
        }

        //抽選後、クレジットが０以下になった場合、ゲームを終了する。
        if (credit <= 0) {
            std::cout << "Thank you for playing! Your final credit is: " << credit << std::endl;
            break;
        }
    }
    return 0;
}