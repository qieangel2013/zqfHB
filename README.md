# zqfHB
    Php Page execution statistics extension
### install zqfHB
    由于依赖c的api调用redis，所以首先需要安装c的api<br/>
    wget https://github.com/redis/hiredis/archive/v0.13.3.tar.gz<br/>
    tar zxvf v0.13.3.tar.gz<br/>
    cd hiredis-0.13.3<br/>
    make<br/>
    mkdir -p /usr/local/include/hiredis  /usr/local/lib<br/>
    cp -a hiredis.h async.h read.h sds.h adapters /usr/local/include/hiredis<br/>
    cp -a libhiredis.so /usr/local/lib/libhiredis.so.0.13<br/>
    cd /usr/local/lib && ln -sf libhiredis.so.0.13 libhiredis.so<br/>
    切换到cd hiredis-0.13.3目录下<br/>
    cp -a libhiredis.a /usr/local/lib<br/>
    mkdir -p /usr/local/lib/pkgconfig<br/>
    cp -a hiredis.pc /usr/local/lib/pkgconfig<br/>
    如果出现libhiredis.so.0.13: cannot open shared object file: No such file or directory in Unknown on line 0<br/>
    vi /etc/ld.so.conf<br/>
    文件末尾添加  /usr/local/lib<br/>
    然后执行ldconfig<br/>
    下面安装本扩展<br/>
    ./phpize<br/>
    ./configure --with-php-config=/usr/local/php/bin/php-config<br/>
    make<br/>
    make install<br/>
    add zqfHB.so to php.ini<br/>
    extension=zqfHB.so<br/>
    [zqfHB]<br/>
    zqfHB.slow_maxtime=10000(单位微妙1s=1000000us,改参数是页面加载超过这个时间会统计)<br/>
    zqfHB.type=1（1代表redis 2代表memcache，由于memcache性能处理有点低，暂时不开放）<br/>
    zqfHB.host=192.168.102.163<br/>
    zqfHB.port=6379<br/>

###use:
       需要把web里的所有文件复制到网站目录下，网站统计可以不在一起
       直接执行http://localhost/web/
###效果图
![](https://github.com/qieangel2013/zqfHB/blob/master/images/img1.png)
![](https://github.com/qieangel2013/zqfHB/blob/master/images/img2.png)
