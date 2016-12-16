# zqfHB
    Php Page execution statistics extension
### install zqfHB
	由于依赖hiredis的c的api调用redis，所以首先需要安装hiredis
	wget https://github.com/redis/hiredis/archive/v0.13.3.tar.gz
	tar zxvf v0.13.3.tar.gz
	cd hiredis-0.13.3
	make
	或者直接make install，建议手动按下面配置
	mkdir -p /usr/local/include/hiredis  /usr/local/lib
	cp -a hiredis.h async.h read.h sds.h adapters /usr/local/include/hiredis
	cp -a libhiredis.so /usr/local/lib/libhiredis.so.0.13
	cd /usr/local/lib && ln -sf libhiredis.so.0.13 libhiredis.so
	切换到cd hiredis-0.13.3目录下
	cp -a libhiredis.a /usr/local/lib
	mkdir -p /usr/local/lib/pkgconfig
	cp -a hiredis.pc /usr/local/lib/pkgconfig
	如果出现libhiredis.so.0.13: cannot open shared object file: No such file or directory in Unknown on line 0
	vi /etc/ld.so.conf
	文件末尾添加  /usr/local/lib
	然后执行ldconfig
	下面安装本扩展
	./phpize
	./configure --with-php-config=/usr/local/php/bin/php-config
	make
	make install
	add zqfHB.so to php.ini
	extension=zqfHB.so
	[zqfHB]
	zqfHB.slow_maxtime=10000(单位微妙1s=1000000us,改参数是页面加载超过这个时间会统计)
	zqfHB.type=1（1代表redis 2代表memcache，由于memcache性能处理有点低，暂时不开放）
	zqfHB.auth=123456(如果redis没有密码，此项不必配置，如果有密码，必须配置此项)
	zqfHB.host=192.168.102.163
	zqfHB.port=6379
###use:
       需要把web里的所有文件复制到网站目录下，网站统计可以不在一起
       直接执行http://localhost/web/
###效果图
![](https://github.com/qieangel2013/zqfHB/blob/master/images/img1.png)
![](https://github.com/qieangel2013/zqfHB/blob/master/images/img2.png)
###交流使用
	交流群：337937322
###如果你对我的辛勤劳动给予肯定，请给我捐赠，你的捐赠是我最大的动力
![](https://github.com/qieangel2013/zys/blob/master/public/images/pay.png)
[项目捐赠列表](https://github.com/qieangel2013/zys/wiki/%E9%A1%B9%E7%9B%AE%E6%8D%90%E8%B5%A0)
