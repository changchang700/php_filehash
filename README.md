# php_filehash
企业微信-微盘-分块累积sha-微盘文件-PHP微盘分片上传扩展-获取分片hash

# 对应文档
https://developer.work.weixin.qq.com/document/path/98004#%E9%99%84%E5%BD%95-%E5%88%86%E5%9D%97%E7%B4%AF%E7%A7%AFsha%E8%AF%B4%E6%98%8E


为了支持大文件上传，将文件进行分块，然后分别上传。

步骤如下：


上传初始化：计算文件的分块（分块大小固定为2M，即2097152字节）的累积sha值（使用sha1算法），提交到企业微信开放平台。如果命中秒传，后续步骤则不需要再进行。
分块上传文件：分别上传分块文件（可并发上传，并发数建议不超过10）
标记上传完成：如果文件上传完整，返回上传成功的fileid

# 编译安装

```
phpize

./configure

make

make install
```
# 安装
修改php.ini，添加扩展filehash.so

# 测试
php test.php

# 返回
待上传文件分块后的hash数组


```
array(3) {
  [0]=>
  string(40) "5186fee37e9f7e77a1f6bea8d4e32638d5186d44"
  [1]=>
  string(40) "cf540965f3190bc0c6367147ab0ac64dc9eff6a0"
  [2]=>
  string(40) "bda1e884e1cb23a5cbdea73d2e00e23baa72d2f5"
}

```
