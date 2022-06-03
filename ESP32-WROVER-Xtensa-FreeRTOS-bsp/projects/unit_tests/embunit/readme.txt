

                        == Embedded Unit ==
               https://sourceforge.jp/projects/embunit/

------------------------------------------------------------------------------

Embedded Unit��C�����Ȥä��Ȥ߹��߷ϳ�ȯ�����Υƥ��ȥ�˥åȥե졼������
����Cɸ��饤�֥���Ȥ�ʤ��ΤǼ¹Ի񸻤ξ��ʤ��������åȴĶ���ư���ǽ�Ǥ���
�ޤ���malloc�ؿ�����Ѥ��Ƥ��ޤ��󡣤��٤ƤΥ��֥������Ȥ�ROM�ΰ�˳��ݤ����
����

------------------------------------------------------------------------------

1.�ǥ��쥯�ȥꡦ�ե����빽��
    [embUnit]
     +- COPYING         : �饤����
     +- makefile        : �ᥤ���ե�����
     +- readme.txt      : ���Υե�����
     +- [embUnit]       : Embedded Unit �饤�֥�꥽����
     +- [msvc]          : MSVC++6.0 Project
     +- [samples]       : Embedded Unit ����ץ륽����
     +- [tests]         : Embedded Unit �ƥ��ȥ�����
     +- [tools]         : �ƥ��ȥ����ɤο��������ġ���


2.�������åȥ����ƥ�ȳ�ȯ�Ķ�

    Embedded Unit�μ¹ԴĶ��Ȥ��Ƽ��Τ�Τ����ꤷ�Ƥ���

    ��2KB�ʾ��ROM�ΰ衢128b�ʾ�Υ����å��ΰ����Ѳ�ǽ�ʴĶ�
    ��ɸ��C�饤�֥�꤬���ѤǤ��ʤ����������ʤ��Ķ�

    �ޤ�����ȯ�Ķ��Ȥ��Ƽ��Τ�Τ����ꤷ�Ƥ���

    ��C����ѥ��餬�Ȥ���Ķ�

    ��­

    Embedded Unit�γ�ȯ�ϼ��δĶ��ǹԤ��Ƥ���

    ��Microsoft Windows XP Professional
    ��VC++.NET or cygwin 1.3.22 + gcc 3.2
	��Microsoft Windows 98
	��VC++6.0
    ��Apple Computer MacOS X 10.1.5
    ��Project Builder 1.1.1 (gcc 2.95.2)


3.����ѥ���
    3.1.UNIX��(cygwin)
        - ɬ�פ˱�����embUnit/config.h���Խ�,�ʲ��Υ��ޥ�ɤ�¹�.

            $ make

        - �ƥ��ȥե졼�༫�ȤΥƥ��Ȥ�¹�.

            $ make test

        - ����ץ��¹�.

            $ make samples

    3.2.MSVC++6.0
        - msvc/embUnit.dsw �򳫤��Ƴƥץ������Ȥ�ӥ�ɤ��Ƥ�������.

    3.3.MacOSX
        - ɬ�פ˱�����embUnit/config.h���Խ�,CC�ѿ���cc���֤��������ޥ�ɼ¹�.

            $ make CC=cc


4.���ݡ��Ȥ��뵡ǽ�γ���

    �����ڥޥ���
      TEST_ASSERT_EQUAL_STRING������ʸ����θ���
      TEST_ASSERT_EQUAL_INT������int���ѿ��θ���
      TEST_ASSERT_NULL������NULL�ݥ��󥿤θ���
      TEST_ASSERT_NOT_NULL��������NULL�ݥ��󥿤θ���
      TEST_ASSERT_MESSAGE���������ڤȥ�å�����
      TEST_ASSERT����������
      TEST_FAIL����������

    ���إ�ѡ��ޥ���
      �ƥ��Ȥ򵭽Ҥ���Τ�ɬ�פʺ�Ȥ�إ�ѡ��ޥ���Ǵ�ά�����ޤ���


5.���襵�ݡ��Ȥ��뵡ǽ�γ���

    ��CUITestRunner
      ���÷���TestRunner�γ�ȯ��ͽ�ꤷ�Ƥ��ޤ���
      ��ɸ��C�饤�֥�꤬�Ȥ���Ķ������ꤷ�Ƴ�ȯ��Ԥ��ޤ���
        ��ǽ�ʤ�ɸ��C�饤�֥���Ȥ�ʤ��С������Ⳬȯ���ޤ���

    ��GUITestRunner
      GUI�����Ѥ���TestRunner�Ǥ���

    ��textui
      Text������XML������Compiler�����Υƥ��ȷ�̤���Ϥ���TestRunner��ȯ��
      �ޤ���


6.���»���

    Embedded Unit�ϥƥ��ȷ�̤ν��Ϥ�ɸ��饤�֥���printf�ؿ�����Ѥ��Ƥ���
    �����⤷ɸ��饤�֥���printf�ؿ�����Ѥ������ʤ����ϡ�

        void stdimpl_print(const char* string)
        ��stdimpl_print�ؿ���ʸ����ν����˲��Ԥ���Ϥ��ʤ��ؿ�

    �Ȥ����ؿ��������������ѥ��륪�ץ�����"-DNO_STDIO_PRINTF"���ɲä��Ƥ�
    ���������⤷����config�إå��ΰʲ��Υ����ȤϤ����Ƥ���������

        /*#define NO_STDIO_PRINTF*/


7.�饤����

    7.1.�饤����
        Embedded Unit��
            MIT/X Consortium License
        �˽�����ΤȤ��ޤ���

    7.2.���ܸ���(����)
        http://xjman.dsl.gr.jp/xf86_3/CPYRIGHT-2.html

    7.3.���ޤ��ޤʥ饤���󥹤Ȥ����ˤĤ��Ƥβ���
        http://www.gnu.org/licenses/license-list.ja.html


------------------------------------------------------------------------------
$Id: readme.txt,v 1.10 2003/09/16 11:41:48 arms22 Exp $
