FROM shanliu/phpext:latest

COPY ./ /root/php/ext/entity/

RUN set -ex \
#xdebug
&& wget  -t0 --no-check-certificate -O ./xdebug-2.8.0beta2.tgz  https://xdebug.org/files/xdebug-2.8.0beta2.tgz \
&& tar zfvx ./xdebug-2.8.0beta2.tgz \
&& cd xdebug-2.8.0beta2 \
&& phpize  \
&& ./configure \
&& make -j 4  \
&& make install \
&& sed -i '$a\zend_extension=xdebug.so' /usr/local/lib/php.ini \
&& rm -rf php/ext/xdebug* \
#entity
&& cd /root/php/ext/entity \
&& composer global config -g repo.packagist composer https://mirrors.aliyun.com/composer/ \
&& cd /root/php/ext/entity/ext \
&& phpize && ./configure && make && make install \
&& sed -i '$a\extension=entity.so' /usr/local/lib/php.ini \
#test env
&& echo >/root/php/ext/entity_mysql.sh \
&& chmod 755 /root/php/ext/entity_mysql.sh \
&& sed -i '$a\apt install -y mysql-server' /root/php/ext/entity_mysql.sh \
&& sed -i '$a\service mysql start' /root/php/ext/entity_mysql.sh \
&& sed -i '$a\mysql -e "use mysql; update user set authentication_string=PASSWORD(\\\"\\\") where User=\\\"root\\\"; update user set plugin=\\\"mysql_native_password\\\";FLUSH PRIVILEGES;create database test;use test; CREATE TABLE address (id int(11) NOT NULL AUTO_INCREMENT,name varchar(32),enname varchar(64) ,acronym varchar(12),code varchar(21) ,add_time int(11) ,PRIMARY KEY (id));"' /root/php/ext/entity_mysql.sh

WORKDIR /root/php/ext/entity

CMD ["/bin/bash"]