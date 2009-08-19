#ifndef CHARSETDETECTOR_H
#define CHARSETDETECTOR_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 编码识别函数库句柄类型定义
 */
typedef void* DETECTOR_HANDLE;

/**
 * 初始化编码识别器
 * 
 * @return 编码识别器句柄
 */
DETECTOR_HANDLE init_charset_detector();

/**
 * 分析一个文本缓冲区的编码
 * 仅能在init_charset_detector()后调用
 * 
 * @param handle   编码识别器句柄
 * @param inBuffer 待分析的文本缓冲区
 * @param len      文本缓冲区的长度（并非字符串长度，文本缓冲区内可包含任意0）
 * 
 * @return 编码字符串，可能的值如下：
 *         gb18030
 *         Big5
 *         UTF-8
 *         ASCII
 *         X-ISO-10646-UCS-4-3412
 *         UTF-16BE
 *         UTF-32BE
 *         X-ISO-10646-UCS-4-2143
 *         UTF-32LE
 *         UTF-16LE
 *         NULL——表示分析不出来
 */
const char* detect_buffer_charset(DETECTOR_HANDLE handle, char *inBuffer, int len);

/**
 * 重置编码分析器状态，此后可以开始下一轮分析
 * 
 * @param handle 编码识别器句柄
 */
void reset_charset_detector(DETECTOR_HANDLE handle);

/**
 * 释放并退出编码识别器
 * 
 * @param handle 编码识别器句柄
 */
void release_charset_detector(DETECTOR_HANDLE handle);

#ifdef __cplusplus
}
#endif

#endif	// CHARSETDETECTOR_H

