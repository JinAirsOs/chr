# chr
C http request
#Install
cd ~ && git clone https://github.com/JinAirsOs/chr.git
cd chr && gcc chr.c -o chr
sudo mv ~/chr/chr /usr/local/bin/chr
#Usage
Http get request example:
chr get www.baidu.com
chr get www.google.com
if has param:
chr get www.example.com?test=1&test=2
Http post request example:
chr post "test=1&test1=2" www.baidu.com
chr post "test=1&test2=2" 127.0.0.1:3000

