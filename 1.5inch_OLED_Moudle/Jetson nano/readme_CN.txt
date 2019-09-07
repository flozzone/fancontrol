/*****************************************************************************
* | File      	:   Readme_CN.txt
* | Author      :   Waveshare team
* | Function    :   Help with use
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2019-06-25
* | Info        :   在这里提供一个中文版本的使用文档，以便你的快速使用
******************************************************************************/
这个文件是帮助您使用本例程。

1.基本信息：
本例程是基于jetson-nano-sd-r32.1.1-2019-05-31系统镜像而开发的，由于目前的系统是没有硬件SPI的，
模拟SPI驱动能力不够，使用I2C开发。
本例程是基于Jetson Nano进行开发的，例程均在Jetson Nano上进行了验证;
本例程使用1.5inch OLED Module模块进行了验证。

2.管脚连接：
管脚连接你可以在Config.py中查看，这里也再重述一次：
OLED   =>    Jetson Nano/RPI(BCM)
VCC    ->    3.3
GND    ->    GND
DIN    ->    2(SDA)
CLK    ->    3(SCL)
CS     ->    8(SPI0_CS0)
DC     ->    24
RST    ->    25


3.基本使用：
由于本工程是一个综合工程，对于使用而言，你可能需要阅读以下内容：
安装对应的库numpy、Pillow和GPIO
python2
    sudo apt-get install python-pip
    sudo pip install numpy
    sudo pip install Pillow
    sudo pip install Jetson.GPIO
    sudo groupadd -f -r gpio
    sudo usermod -a -G gpio your_user_name
    vsudo udevadm control --reload-rules && sudo udevadm trigger
python3
    sudo apt-get install python-pip3
    sudo pip3 install numpy
    sudo pip3 install Pillow
    如果安装Pillow出错
        sudo apt-get install python3-dev python3-setuptools libtiff5-dev zlib1g-dev libfreetype6-dev liblcms2-dev libwebp-dev libharfbuzz-dev libfribidi-dev  tcl8.6-dev tk8.6-dev python3-tk
    sudo pip3 install Jetson.GPIO
    sudo groupadd -f -r gpio
    sudo usermod -a -G gpio your_user_name
    vsudo udevadm control --reload-rules && sudo udevadm trigger

然后你需要执行：
python2
    运行：sudo python main.py
python3
     运行：sudo python3 main.py
