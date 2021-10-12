# FileUtil实现细节
- 手动设置文件缓冲区的大小为 64KB
- 使用fwrite_unblocked()获得更快的性能