# zqfHB
Php red envelope expansion
### install zqfHB
    ./phpize
    ./configure --with-php-config=/usr/local/php/bin/php-config
    make
    make install
    add zqfHB.so to php.ini
###use:
        The first parameter is the total amount of red envelopes, the second parameter number of red envelopes, 
        the third parameter default to fight gas red envelopes, set to 1, then the ordinary red envelope
        Fight hand gas red envelopes
        hongbao(10,8);then hongbao(10,8,0);then the array is 
        Array ( [0] => 1.33 [1] => 1.02 [2] => 1.28 [3] => 0.44 [4] => 1.37 [5] => 0.81 [6] => 1.81 [7] => 1.94 )
        Ordinary red envelopes, the same amount of each set the third parameter
        hongbao(10,8,1);then the array is 
        Array ( [0] => 1.25 [1] => 1.25 [2] => 1.25 [3] => 1.25 [4] => 1.25 [5] => 1.25 [6] => 1.25 [7] => 1.25 )
        var_dump($hongb);
