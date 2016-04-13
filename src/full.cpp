/*
 * full.cpp
 *
 *  Created on: Apr 8, 2016
 *      Author: jcalvopi
 */



    #include <iostream>
    using namespace std;

    char Nombres[6][6] = {{"jose"},{"pedro"},{"luis"},{"omar"},{"etc"},{0}};
   // char nombre[5];
    int main()
    {
        for(int i = 0; i < 6;++i){
            for(int e = 0; e < 6;++e){
                cout<<Nombres[i][e];
               // cout << Nombres[i];
           }
            cout<<"\n";
        }
        cin.get();
        return 0;
    }
