参考了OpenGL渲染管线流程以及项目tinyrender和GAMES101作业代码框架，纯C++构建的软光栅渲染器。

同时还加入了抗锯齿MSAA的小trick(4倍超采样)，可根据指令设置是否开启

## 使用方法

编译main.cpp文件，将exe程序放在根目录下，使用指令格式：

 main.exe   <model> <shading_method> <outputfile> <msaa>

例如：

main.exe diablo3 gourand output msaa (开启msaa，不写默认不开启)

## 效果展示

### Flat shading

![flat example](https://github.com/CrtlCvWei/MyTinyRender/blob/master/expample/flat.jpg)

### Gouraud shading

![example](https://github.com/CrtlCvWei/MyTinyRender/blob/master/expample/gouraud1.jpg)

!https://github.com/CrtlCvWei/MyTinyRender/blob/master/expample/gouraud1.jpg

![example]([https://github.com/CrtlCvWei/MyTinyRender/blob/master/expample/gouraud2.jpg](https://github.com/CrtlCvWei/MyTinyRender/blob/master/expample/gouraud1.jpg))

### Blinn Phong 光照-无阴影

![example image]([https://github.com/CrtlCvWei/MyTinyRender/blob/master/expample/blinn2.jpg](https://github.com/CrtlCvWei/MyTinyRender/blob/master/expample/blinn1.jpg))

!https://github.com/CrtlCvWei/MyTinyRender/blob/master/expample/gouraud1.jpg

![example image](https://github.com/CrtlCvWei/MyTinyRender/blob/master/expample/blinn1.jpg)

### Blinn Phong+阴影 ShadowMap

![example image](https://github.com/CrtlCvWei/MyTinyRender/blob/master/expample/shadow1.jpg)

!https://github.com/CrtlCvWei/MyTinyRender/blob/master/expample/gouraud1.jpg

![example image]([https://github.com/CrtlCvWei/MyTinyRender/blob/master/expample/shadow2.jpg](https://github.com/CrtlCvWei/MyTinyRender/blob/master/expample/shadow1.jpg))

### MSAA o/w

开启MSAA，图像高频部分展示：

![example image](https://github.com/CrtlCvWei/MyTinyRender/blob/master/expample/msaa_1.jpg)

未开启时MSAA，图像高频部分展示：

![example image](https://github.com/CrtlCvWei/MyTinyRender/blob/master/expample/msaa_no.jpg)