<h1 align="center" >物联网-ESP32🎉</h1>

<div align="center">
  <p>
    <strong>本仓库是个人对ESP32-S3的开发经验总结，希望对你有所帮助</strong><br>


  </p>
</div>


## 开发环境

- 平台: VSCode
- 插件: PlatformIO IDE

## 如何开始

1. 安装VSCode和PlatformIO IDE插件。
2. 克隆这个仓库到本机。

```bash
git clone https://github.com/yourusername/yourrepository.git
``` 
3. 打开VSCode，然后打开克隆的项目文件夹。你可以开始编写代码了


## 项目结构

```
├─.pio	    	    (组件库)
│  ├─build		(编译生成文件)
│  ├─libdeps	    (项目所依赖的所有库文件)
├─.vscode		(vscode配置文件)
├─include		(暂时没有用到：项目需要的头文件)
├─lib			(个人开发的程序库 )
│  ├─mqtt		    (mqtt库)
│  ├─led    		(led库)
│  ├─servo	    	(servo库)
├─src			(源文件目录)

```

## 个人开发库详解

### [1. mqtt库](./lib/mqtt)
> 功能：连接华为云IoTDA平台，上报属性及命令下发。

### [2. led库](./lib/led)
> 功能：封装控制LED灯的开关。

### [3. servo库](./lib/servo)
> 功能：封装控制舵机的角度。

## 贡献
 如果你想为这个项目做出贡献，欢迎提交Pull Request。


## 许可证
这个项目使用MIT许可证，详情请见LICENSE文件。