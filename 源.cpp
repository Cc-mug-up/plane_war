//飞机大战
//EasyX
#include<iostream>
#include<graphics.h>
#include<vector>
#include<conio.h>
using namespace std;
const int swidth = 600;
const int sheight = 800;
const unsigned long long HP = 4;
const unsigned long long hurttime = 200;//ms
//背景，敌机，子弹，敌军子弹，英雄，击杀数显示
class BK {
public:
	BK(IMAGE &img, IMAGE& img1):img(img),pause1(img1), y(-sheight) {}
	void show() {
		if (y >= 0) {
			y = -sheight;
		}
		//背景速度
		y += 4;
		//cout << y << endl;
		putimage(0, y, &img);
		putimage(0, 0, &pause1);
	}
private:
	IMAGE &img;
	IMAGE &pause1;
	int y;
};

class Hero {
public:
	Hero(IMAGE& img):img(img),hp(HP){
		rect.left = swidth / 2 - img.getwidth() / 2;
		rect.top = sheight - img.getheight();
		rect.right = rect.left + img.getwidth();
		rect.bottom = rect.top + img.getheight();
	}
	void Show() {
		setlinecolor(RED);
		setlinestyle(PS_SOLID, 3, 0);
		putimage(rect.left, rect.top, &img);
		line(rect.left, rect.top-3, rect.left + (img.getwidth() / HP * hp), rect.top-3);
	}
	void Control() {
		//获取鼠标消息
		ExMessage msg;
		if (peekmessage(&msg, EM_MOUSE)) {
			rect.left = msg.x - img.getwidth() / 2;
			rect.right = rect.left + img.getwidth();
			rect.top = msg.y - img.getheight() / 2;
			rect.bottom = rect.top + img.getheight();
		}
	}
	bool hurt() {
		hp--;
		return (hp == 0) ? false : true;
	}
	RECT& GetRect() {
		return rect;
	}
private:
	IMAGE& img;
	RECT rect;
	unsigned long long hp;
};

class Enemy {
public:
	Enemy(IMAGE& img,int x,IMAGE* boom):img(img),isdle(false),boomsum(0) {
		selfboom[0] = boom[0];
		selfboom[1] = boom[1];
		selfboom[2] = boom[2];
		selfboom[3] = boom[3];
		rect.left = x;
		rect.right = rect.left + img.getwidth();
		rect.top = -img.getheight();
		rect.bottom = 0;
	}
	bool show() {
		if (isdle) {
			if(boomsum >= 4) {
				return false;
			}
			putimage(rect.left, rect.top, selfboom+boomsum);
			boomsum++;
			return true;
		}
		if (rect.top >= sheight) {
			return false;
		}
		rect.top += 3; 
		rect.bottom += 3;
		putimage(rect.left,rect.top,&img);
		return true;
	}
	void dle() {
		isdle = true;
	}
	RECT& GetRect() {
		return rect;
	}
private:
	IMAGE& img;
	RECT rect;
	IMAGE selfboom[4];
	bool isdle;
	int boomsum;
};

class Bullet {
public:
	Bullet(IMAGE& img,RECT& pr):img(img) {
		rect1.left = pr.left + (pr.right - pr.left) / 2 - img.getwidth() / 2;
		rect1.right = rect1.left + img.getwidth();
		rect1.top = pr.top - img.getheight();
		rect1.bottom = pr.top;

		rect2.top = rect1.top;
		rect2.bottom = rect1.bottom;
		rect2.left = rect1.left - 3 * (rect1.right - rect1.left);
		rect2.right = rect2.left + (rect1.right - rect1.left);

		rect3.top = rect1.top;
		rect3.bottom = rect1.bottom;
		rect3.left = rect1.left + 3 * (rect1.right - rect1.left);
		rect3.right = rect3.left + (rect1.right - rect1.left);
	}
	bool show() {
		if (rect1.bottom <= 0|| rect2.bottom <= 0|| rect3.bottom <= 0) {
			return false;
		}
		rect1.top -= 3;
		rect1.bottom -= 3;
		rect2.top -= 3;
		rect2.bottom -= 3;
		rect3.top -= 3;
		rect3.bottom -= 3;
		putimage(rect1.left, rect1.top, &img);
		putimage(rect2.left, rect2.top, &img);
		putimage(rect3.left, rect3.top, &img);
		return true;
	}
	RECT& GetRect1() {
		return rect1;
	}
	RECT& GetRect2() {
		return rect2;
	}
	RECT& GetRect3() {
		return rect3;
	}
protected:
	IMAGE& img;
	RECT rect1;
	RECT rect2;
	RECT rect3;
};

class eBullet :public Bullet{
public:
	eBullet(IMAGE& img, RECT& pr) :
		Bullet(img,pr) {
		rect1.left = pr.left + (pr.right - pr.left) / 2 - img.getwidth() / 2;
		rect1.right = rect1.left + img.getwidth();
		rect1.top = pr.bottom;
		rect1.bottom = rect1.top + img.getheight();
	}
	bool show() {
		if (rect1.top >= sheight) {
			return false;
		}
		rect1.top += 5;
		rect1.bottom += 5;
		putimage(rect1.left, rect1.top, &img);
		return true;
	}
};

class kills {
public:
	kills() {
		killnum = 0;
	}
	void show() {
		_stprintf_s(str, 128, _T("击杀数：%llu"), killnum);
		settextcolor(RED);
		outtextxy(0, sheight-textheight(str), str);
	}
	void update(unsigned long long &n) {
		killnum = n;
	}
private:
	TCHAR* str = new TCHAR[128];
    unsigned long long killnum;
};

//矩形碰撞检测
bool RectDuangRect(RECT& r1, RECT& r2) {

	RECT r;
	r.left = r1.left - (r2.right - r2.left);
	r.right = r1.right;
	r.top = r1.top - (r2.bottom - r2.top);
	r.bottom = r1.bottom;
	//if ((r.left <= r2.left && r2.left <= r.right && r.top <= r2.top && r2.top <= r.bottom)) cout << "success" << endl;
	return (r.left <= r2.left && r2.left <= r.right && r.top <= r2.top && r2.top <= r.bottom);

}

//一个开始界面 
void Welcome() {
	//文本
	LPCTSTR title = _T("飞机大战");
	LPCTSTR tplay = _T("开始游戏");
	LPCTSTR texit = _T("退出游戏");
	RECT tplayr, texitr;
	//批量绘画
	BeginBatchDraw();
	//设置背景颜色
	setbkcolor(BLACK);
	//清屏
	cleardevice();
	//设置文本颜色
	settextcolor(WHITE);
	//设置文本字体
	settextstyle(40, 0, _T("黑体"));
	//绘制文本title 
	outtextxy(swidth / 2 - textwidth(title) / 2, sheight / 5, title);
	//设置文本字体
	settextstyle(30, 0, _T("黑体"));
	//设置文本样式
	tplayr.left = swidth / 2 - textwidth(tplay) / 2;
	tplayr.top = sheight / 5 * 2;
	tplayr.right = tplayr.left + textwidth(tplay);
	tplayr.bottom = tplayr.top + textheight(tplay);
	texitr.left = swidth / 2 - textwidth(texit) / 2;
	texitr.top = sheight / 5 * 2.5;
	texitr.right = texitr.left + textwidth(texit);
	texitr.bottom = texitr.top + textheight(texit);
	//绘制文本tplay
	outtextxy(tplayr.left, tplayr.top, tplay);
	//绘制文本texit
	outtextxy(texitr.left, texitr.top, texit);
	//结束批量绘画
	EndBatchDraw();
	//循环监听鼠标消息
	while (true) {
		//获取鼠标消息
		ExMessage msg;
		getmessage(&msg, EM_MOUSE);
		//判断鼠标左键是否按下
		if (msg.lbutton) {
			//判断鼠标是否在文本tplayr范围内
			if (msg.x >= tplayr.left && msg.x <= tplayr.right && msg.y >= tplayr.top && msg.y <= tplayr.bottom) {
				//开始游戏
				return;
			}
			//判断鼠标是否在文本texitr范围内
			else if (msg.x >= texitr.left && msg.x <= texitr.right && msg.y >= texitr.top && msg.y <= texitr.bottom) {
				//退出游戏
				exit(0);
			}
		}
	}
}
//击杀数界面
void Over(unsigned long long & kill) {
	TCHAR* str = new TCHAR[128];
	_stprintf_s(str,128,_T("最终击杀数：%llu"),kill);
	settextcolor(RED);
	outtextxy(swidth / 2 - 78, 1.8 * sheight / 5, _T("你已被击毁"));
	outtextxy(swidth/2-textwidth(str)/2,2*sheight/5,str);
	//键盘事件(按Enter返回)
	LPCTSTR info = _T("按Enter键返回");
	settextstyle(20, 0, _T("黑体"));
	outtextxy(swidth - textwidth(info), 0, info);
	while (true) {
		ExMessage mess;
		getmessage(&mess,EM_KEY);
		if (mess.vkcode == 0x0D) {
			return;
		}
	}
}
//添加敌人
bool AddEnemy(vector<Enemy*>& es,IMAGE& enemyimg,IMAGE* boom) {
	Enemy *e = new Enemy(enemyimg,abs(rand())%(swidth-enemyimg.getwidth()),boom);
	for (auto& it : es) {
		if (RectDuangRect(it->GetRect(), e->GetRect())) {
			delete e;
			return false;
		}
	}
	es.push_back(e);
	return true;
}
//
//游戏主函数
bool play() {
	//暂停状态
	bool paused = false;
	setbkcolor(WHITE);
	cleardevice();
	bool is_play = true;
	IMAGE heroimg, enemyimg, bkimg, bulletimg1,bulletimg2,pause1,pause2;
	IMAGE eboom[4];
	//加载图片
	loadimage(&heroimg, _T("D:\\vs2022c++项目\\plane_war\\images\\me1.png"),0,0);
	loadimage(&enemyimg, _T("D:\\vs2022c++项目\\plane_war\\images\\enemy1.png"));
	loadimage(&bkimg, _T("D:\\vs2022c++项目\\plane_war\\images\\bk2.png"),swidth,sheight*2);
	loadimage(&bulletimg1, _T("D:\\vs2022c++项目\\plane_war\\images\\bullet1.png"));
	loadimage(&bulletimg2, _T("D:\\vs2022c++项目\\plane_war\\images\\bullet2.png"));
	loadimage(&eboom[0], _T("D:\\vs2022c++项目\\plane_war\\images\\enemy1_down1.png"));
	loadimage(&eboom[1], _T("D:\\vs2022c++项目\\plane_war\\images\\enemy1_down2.png"));
	loadimage(&eboom[2], _T("D:\\vs2022c++项目\\plane_war\\images\\enemy1_down3.png"));
	loadimage(&eboom[3], _T("D:\\vs2022c++项目\\plane_war\\images\\enemy1_down4.png"));
	loadimage(&pause1, _T("D:\\vs2022c++项目\\plane_war\\images\\pause_nor.png"));
	loadimage(&pause2, _T("D:\\vs2022c++项目\\plane_war\\images\\resume_nor.png")); 
	//加载背景
	BK bk=BK(bkimg,pause1);
	//加载英雄
	Hero hero=Hero(heroimg);
	//加载敌人
	vector<Enemy*>es;
	//加载子弹
	vector<Bullet*>bs;
	vector<eBullet*>ebs;
	//击杀数
	//kills k;
	clock_t hurtlast = clock();
	unsigned long long kill=0;
	for (int i = 1; i <= 5; i++) {
		AddEnemy(es,enemyimg,eboom);
	}
	//子弹更新率
	int bsing = 0;
	while (is_play) {
		BeginBatchDraw();
		//鼠标右键读取暂停状态
		ExMessage msg;
		if (peekmessage(&msg, EM_MOUSE)) {
			if (msg.message == WM_RBUTTONDOWN) {
				paused = !paused;  // 切换暂停状态
				if (paused) {
					settextcolor(RED);
					putimage(0,0, &pause2);
					EndBatchDraw();
				}
			}
		}
		//flushmessage();
		if (paused) {
			Sleep(50);
			continue;  // 跳过游戏逻辑更新
		}
		bsing++;
		//子弹发射（两帧一发）
		if (bsing % 15 == 0) {
			bs.push_back(new Bullet(bulletimg1,hero.GetRect()));
		}
		if (bsing == 60) {
			bsing = 0;
			for (auto& i : es) {
				ebs.push_back(new eBullet(bulletimg2, i->GetRect()));
			}
		}
		bk.show();
		Sleep(2);
		hero.Control();
	    flushmessage();
		Sleep(2);
		hero.Show();
		//k.show();
		//Sleep(2);

		auto bsit = bs.begin();
		while (bsit != bs.end()) {
			if (!(*bsit)->show()) {
				bsit=bs.erase(bsit);
			}
			if(bsit!=bs.end()) bsit++;
		}
		auto ebsit = ebs.begin();
		while (ebsit != ebs.end()) {
			bool shouldErase = false;

			// 检测碰撞
			if (RectDuangRect((*ebsit)->GetRect1(), hero.GetRect())) {
				if (clock() - hurtlast >= hurttime) {
					is_play = hero.hurt();
					hurtlast = clock();
				}
				shouldErase = true;
			}
			// 检测子弹是否超出屏幕
			else if (!(*ebsit)->show()) {
				shouldErase = true;
			}

			// 统一处理删除逻辑
			if (shouldErase) {
				delete* ebsit;
				ebsit = ebs.erase(ebsit);  // 使用返回值更新迭代器
			}
			else {
				++ebsit;  // 仅当未删除时递增
			}
		}
		auto it = es.begin();
		while (it != es.end()) {
			if(RectDuangRect((*it)->GetRect(), hero.GetRect())) 
				if (clock() - hurtlast >= hurttime) {
				is_play = hero.hurt();
				hurtlast = clock();
			    }
			auto bit=bs.begin();
			while (bit != bs.end()) {
				if (RectDuangRect((*bit)->GetRect1(),(*it)->GetRect())||
					RectDuangRect((*bit)->GetRect2(), (*it)->GetRect())||
					RectDuangRect((*bit)->GetRect3(), (*it)->GetRect())) {
					(*it)->dle();
					//(*it)->show();
					delete (*bit);
					bs.erase(bit);
					kill++;
					break;
				}
				bit++;
			}
			if (!(*it)->show()) {
				//Sleep(1);
				delete (*it);
				it=es.erase(it);
			}
			if(it!=es.end()) it++;
		}
		for (int i = 0; i < 5 - es.size(); i++) {
			AddEnemy(es, enemyimg,eboom);
		}
		//k.update(kill);
		EndBatchDraw();
		//FlushBatchDraw();
		//cout << kill << endl;
	}
	Over(kill);
	return true;
}

int main() {
	//easyX初始化
	initgraph(swidth, sheight, EX_SHOWCONSOLE | EX_NOCLOSE);
	bool is_live = true;
	while (is_live) {
		Welcome();
		is_live = play();
	}

	return 0;
}