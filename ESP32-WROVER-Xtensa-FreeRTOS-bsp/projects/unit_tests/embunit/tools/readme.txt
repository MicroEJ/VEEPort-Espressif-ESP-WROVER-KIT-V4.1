
1.����

    ���Ӳ�Ƹ�������Ϥϲ�Ƹ�ץ������ȤǺ��줿CUnit�б��ƥ��ȥ����ɿ���
    �����ġ���Ǥ���
    ���Τ��Ӳ�Ƹ�������Ϥ��Ǥ��Ф������ɤ�Embedded Unit�Ѥ˽񤭴����ޤ�����

    ���ꥸ�ʥ�Τ��Ӳ�Ƹ�������Ϥϰʲ���URL���������ǽ�Ǥ���

    ��Ƹ�ץ�������
    http://sourceforge.jp/projects/cuppa/

    ��Ƹ(CppUnit PreProcess Aid)
    http://www.unittest.org/


2.�ե����빽��

    [tools]
    +- readme.txt   :���Υե�����
    +- makefile     :�ᥤ���ե�����
    +- COPYING      :���ɽ��
    +- [tbcuppa]    :���Ӳ�Ƹ��������������
    +- [tbuma]      :�����ϲ�������������


3.����ѥ���

    tools�ǥ��쥯�Ȥ˰�ư����'make'���ޥ�ɤ�¹Ԥ��Ƥ���������
    tools�ǥ��쥯�ȥ�˰ʲ��Σ��ĤΥ��ץꥱ������󤬺�������ޤ���

    tcuppa  ���ƥ��ȥ��롼�פο������������ޤ���
    bcuppa  ��tcuppa���������줿�ƥ��ȥ��롼�פ�¹Ԥ���main��������ޤ���
    tuma    ��tcuppa���������줿�ƥ��ȥ��롼�פ˥ƥ��Ȥ��ɲä��ޤ���
    buma    ��bcuppa���������줿main�˥ƥ��ȼ¹ԥ����ɤ��ɲä��ޤ���


4.�Ȥ���

4.1.tcuppa
    My.h���������Ƥ���ؿ���ƥ��Ȥ���ƥ��ȥ��롼�� MyTest ���������������

    $ tcuppa My.h MyTest testXxx testYyy

    �����Ϥ��롥���������My.h�򥤥󥯥롼�ɤ����Υƥ��ȴؿ�

        static void testXxx(void)
        static void testYyy(void)

    ���������MyTest.c ����������롥�إå��Ͼ�ά��ǽ,�ޤ�ʣ�������ǽ.

4.2.bcuppa
    bcuppa��tcuppa�ˤ�ä��������줿�ƥ��ȥ��롼�פ�缡�¹Ԥ���
    �ᥤ��롼������������ޤ���
    ��ۤɺ������� MyTest ��¹Ԥ��륳���� AllTests ����������ˤ�

    $ bcuppa AllTests MyTest

    �����Ϥ��롥�ޤ������ȥǥ��쥯�ȥ�� ATest.c BTest.c�Ⱥߤä����

    $ bcuppa AllTests *Test.c

    �Τ褦�ʥ��ޥ�ɤλ��꤬��ǽ�Ǥ���

4.3.tuma
    tcuppa���������줿�ƥ��ȥ��롼�פ˥ƥ��Ȥ��ɲä��ޤ���
    ��Ϥ���ۤɺ������� MyTest �˥ƥ��� testZzz ���ɲä�������硢

    $ tuma MyTest testZzz

    �����Ϥ��ޤ���

4.4.buma
    bcuppa���������줿 AllTests �˼¹ԥ����ɤ��ɲä��ޤ���
    ������ YourTest �ȸ����ƥ��ȥ��롼�פ��������ޤ�.

    $ tcuppa YourTest testXxx testYyy

    ������ YourTest �� AllTests ���ɲä��ޤ���

    $ buma AllTests YourTest

------------------------------------------------------------------------------
$Id: readme.txt,v 1.1 2003/09/02 12:07:44 arms22 Exp $
