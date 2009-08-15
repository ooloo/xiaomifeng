#ifndef CHARSETDETECTOR_H
#define CHARSETDETECTOR_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * ����ʶ�����������Ͷ���
 */
typedef void* DETECTOR_HANDLE;

/**
 * ��ʼ������ʶ����
 * 
 * @return ����ʶ�������
 */
DETECTOR_HANDLE init_charset_detector();

/**
 * ����һ���ı��������ı���
 * ������init_charset_detector()�����
 * 
 * @param handle   ����ʶ�������
 * @param inBuffer ���������ı�������
 * @param len      �ı��������ĳ��ȣ������ַ������ȣ��ı��������ڿɰ�������0��
 * 
 * @return �����ַ��������ܵ�ֵ���£�
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
 *         NULL������ʾ����������
 */
const char* detect_buffer_charset(DETECTOR_HANDLE handle, char *inBuffer, int len);

/**
 * ���ñ��������״̬���˺���Կ�ʼ��һ�ַ���
 * 
 * @param handle ����ʶ�������
 */
void reset_charset_detector(DETECTOR_HANDLE handle);

/**
 * �ͷŲ��˳�����ʶ����
 * 
 * @param handle ����ʶ�������
 */
void release_charset_detector(DETECTOR_HANDLE handle);

#ifdef __cplusplus
}
#endif

#endif	// CHARSETDETECTOR_H

