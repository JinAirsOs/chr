# chr
C http request
#Install
cd ~ && git clone https://github.com/JinAirsOs/chr.git<br>
cd chr && gcc chr.c -o chr<br>
sudo mv ~/chr/chr /usr/local/bin/chr<br>
#Usage
Http get request example:<br>
chr get www.baidu.com<br>
chr get www.google.com<br>
if has param:<br>
chr get www.example.com?test=1&test=2<br>
Http post request example:<br>
chr post "test=1&test1=2" www.baidu.com<br>
chr post "test=1&test2=2" 127.0.0.1:3000<br>

