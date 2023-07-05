# netsender
通用网络发送模块

## 简单说明
在嵌入式系统的开发过程中,经常要写一些传输数据的小模块,每次都要从零开始,处理socket相关的操作,实在有点麻烦.
因此,这个库里, 把我平常碰到的问题,都集中在一起,通过不断迭代,最终会得到一个通用的网络程序库.


## 使用方式
上层的应用不用再关注socket的细节,只要按如下步骤操作:
1. 使用netsender创建一个实例,里面指定是服务器端/客户端,
  - 指定使用的协议类型,tcp/udp.
  - 指定是服务器端/客户端.
  - 指定连接的服务器.如果是服务器端, 可以在这个参数里传递绑定的网卡; 如果是客户端,需要指定服务器servername.服务器名可以是IP地址,也可以是域名.如果是域名,需要确保系统里已经配置好了dns服务系统
  - 指定使用的端口.
  - 指定socket的特殊参数.比如我曾经碰到过的粘包问题.需要在创建socket时,把nag算法关闭.
2. 继承一个数据处理协议的子类,实现相关的数据逻辑.把第一步创建的netsender实例与t处理协议实例关联.


## 关于服务器端
编写网络程序一段时间后,赫然发现,自己开发的都是客户端程序,服务器端的开发几乎一片空白.

恶补了一段时间的服务器端开发后, 本想用epoll模型的, 但是发现有一些嵌入式开发环境里默认没有包含epoll开发库.

为了做到能够在多个嵌入式平台里使用,对每个连接的客户端都生成一个线程来处理.

好在嵌入式系统里当前阶段还没碰到需要处理超多连接的情况.现在先如此实现.
