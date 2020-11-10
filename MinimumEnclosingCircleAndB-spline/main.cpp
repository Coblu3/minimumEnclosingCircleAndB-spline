#include <graphics.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

//Akif Demirel 190201011



//değişkenler
const double Sonsuz = 9999999999999;
//Bir araligin uzunluguna gore olcek
const int olcek = 16;
int N;
int splineUzunluk=0;
double splinex[1000];
double spliney[1000];

//Nokta kimligi
struct Nokta
{
    double X, Y;
};
//Cember kimligi
struct Cember
{
    struct Nokta C;
    double R;
};


//Koordinat duzlemini cizen fonksiyon
void koordinat_ciz()
{
    //1000 e 1000 lik pencere
    initwindow(1000,1000);
    setcolor(BLUE);
    //x ekseni
    line(0,500,1000,500);
    //y ekseni
    line(500,1000,500,0);
    settextstyle(SMALL_FONT,HORIZ_DIR,5);
    for(int i = 0; i<33; i++)
    {
        char result[12];
        sprintf(result, "%d", i);
        //Koordinat duzleminde gosterilen sayilar
        outtextxy(500+i*olcek,500,result);
        //Daha belirgin gozukmesi icin pixel
        putpixel(500+i*olcek,500,YELLOW);

        outtextxy(500,500+i*olcek,result);
        putpixel(500,500+i*olcek,YELLOW);

        outtextxy(500,500-i*olcek,result);
        putpixel(500,500-i*olcek,YELLOW);

        outtextxy(500-i*olcek,500,result);
        putpixel(500-i*olcek,500,YELLOW);

    }
}

//Txt dosyasindaki sayilari duzlemde gosteren fonksiyon
void noktalari_goster(struct Nokta P[])
{
    for(int i =0; i<N; i++)
    {
        setlinestyle(4,3,1);
        setcolor(GREEN);
        //Noktalarin x ve y sini daha iyi gorebilmek icin
        //Noktalardan duzlemlere indirilen kesikli line

        line(500+P[i].X*olcek,500-P[i].Y*olcek,500,500-P[i].Y*olcek);
        line(500+P[i].X*olcek,500-P[i].Y*olcek,500+P[i].X*olcek,500);

        setlinestyle(0,0,1);
        setcolor(WHITE);
        //Noktalarin daha belirgin gozukmesi icin daire kullanildi
        circle(500+P[i].X*olcek,500-P[i].Y*olcek,2);



    }
    setlinestyle(0,0,1);
}


//iki nokta arasindaki uzaklik bulma formülü fonksiyonu
double uzaklik(const struct Nokta a, const struct Nokta b)
{
    return sqrt(pow(a.X-b.X,2)+pow(a.Y-b.Y,2));

};

//Noktanin cemberin icinde olup olmadigini kontrol eden fonksiyon
int icinde_mi(const struct Cember c, const struct Nokta p)
{
    //Eger cemberin merkezi ile nokta arasindaki uzaklik cemberin r sinden buyuk ise
    //Nokta cember disinda olmak zorundadir
    if (uzaklik(c.C, p) <= c.R)
    {
        return 1;
    }
    return 0;
}
//2 Nokta ile cember olusturma fonksiyonu
struct Cember iki_noktali_cember_olustur(const struct Nokta a, const struct Nokta b)
{
    //2 noktanin orta noktasi cember merkezi olarak kabul edilir ve uzakliklarinin yarisi cemberin r sidir
    struct Nokta A = {(a.X+b.X)/2.0,(a.Y+b.Y)/2.0};
    struct Cember S = {A,uzaklik(a,b)/2.0};
    return S;
};

//Cemberin merkezini bulan fonksiyon
//Cemberdeki 3 noktadan olusan ucgenin kenarortaylarinin kesisimi cemberin merkezidir
struct Nokta Cemberin_orta_Noktasi(double bx, double by, double cx, double cy)
{
    double B = bx * bx + by * by;
    double C = cx * cx + cy * cy;
    double D = bx * cy - by * cx;
    struct Nokta n = { (cy * B - by * C) / (2 * D), (bx * C - cx * B) / (2 * D) };
    return n;
}
//Noktalarin cemberin icinde mi disinda mi oldugunu kontrol eden kontrol fonksiyonum
int cember_dogru_mu(const struct Cember c, const struct Nokta P[])
{
    for (int i=0; i<N; i++)
    {

        if (!icinde_mi(c, P[i]))
            return 0;

    }
    return 1;
}
//Uc nokta ile cember olusturma fonksiyonu
struct Cember uc_noktali_cember_olustur(const struct Nokta P[],struct Cember c)
{
    struct Cember tmp_cember;
    //Noktalar arasinda dolasim
    for(int i =0; i<N; i++)
    {
        for(int j = i+1 ; j<N; j++)
        {
            for(int k= j+1; k<N; k++ )
            {
                //Gecici cemberi olusturuyoruz
                tmp_cember.C = Cemberin_orta_Noktasi(P[j].X - P[i].X, P[j].Y - P[i].Y, P[k].X - P[i].X, P[k].Y - P[i].Y);
                tmp_cember.C.X += P[i].X;
                tmp_cember.C.Y += P[i].Y;
                //Cemberin r sini buluyoruz
                tmp_cember.R = uzaklik(P[i],tmp_cember.C);
                //Eger gecici cember onceki cemberden daha kucuk ise
                if(tmp_cember.R < c.R && cember_dogru_mu(tmp_cember,P) )
                {
                    //gecici cemberi atıyoruz cember yerine
                    c=tmp_cember;
                }
            }
        }
    }
    return c;
};




//minimum enclosing circle ı buldugumuz fonksiyon
struct Cember minimum_cember(struct Nokta P[],int N)
{
    //Sonsuz r ye sahip bir cember olusturuyoruz
    struct Cember C = {{0,0},Sonsuz};
    //Noktalar arasi dolasma
    for (int i=0; i<N; i++)
    {
        for(int j=i+1; j<N; j++)
        {

            struct Cember gecici = iki_noktali_cember_olustur(P[i],P[j]);
            //Eger gecici cember bir onceki cemberden kucuk ise
            if (gecici.R<C.R && cember_dogru_mu(gecici,P))
            {
                //Gecici cemberi C ye atıyoruz
                C = gecici;
            }



        }
    }
    C = uc_noktali_cember_olustur(P,C);

    return C;

};
//B-spline algoritmasini uyguladigimiz fonksiyon
void b_spline_algorithm(struct Nokta p1,struct Nokta p2,struct Nokta p3,struct Nokta p4,int n)
{
    double a[5];
    double b[5];
    //B spline algoritmasi matrisden cevirdigimiz sekilde
    a[0] = (-(p1.X*olcek) + 3 * (p2.X*olcek) - 3 * (p3.X*olcek) + (p4.X*olcek)) / 6.0;
    a[1] = (3 * (p1.X*olcek) - 6 * (p2.X*olcek) + 3 * p3.X*olcek) / 6.0;
    a[2] = (-3 * (p1.X*olcek) + 3 * (p3.X*olcek)) / 6.0;
    a[3] = ((p1.X*olcek) + 4 * (p2.X*olcek) + (p3.X*olcek)) / 6.0;
    b[0] = (-(p1.Y*olcek) + 3 * (p2.Y*olcek) - 3 * (p3.Y*olcek) + (p4.Y*olcek)) / 6.0;
    b[1] = (3 * (p1.Y*olcek) - 6 * (p2.Y*olcek) + 3 * (p3.Y*olcek)) / 6.0;
    b[2] = (-3 * (p1.Y*olcek) + 3 * (p3.Y*olcek)) / 6.0;
    b[3] = ((p1.Y*olcek) + 4 * (p2.Y*olcek) + (p3.Y*olcek)) / 6.0;

    for (int i = 0; i <= n-1; i++)
    {
        float fi = float(i);
        float fn = float(n);
        float  t = fi/fn;
        splinex[splineUzunluk] =  (a[2] + t * (a[1] + t * a[0]))*t+a[3]  ;
        spliney[splineUzunluk] =    (b[2] + t * (b[1] + t * b[0]))*t+b[3] ;
        splineUzunluk++;



    }

}
//Noktalarla b-spline egrisini olusturan fonksiyon
void b_spline(struct Nokta P[])
{
    struct Nokta temp;
    double I;
    //listeyi x e gore siralama
    for(int j=0; j<N; j++)
    {
        for(int k=j+1; k<N; k++)
        {
            if(P[j].X>P[k].X)
            {
                temp=P[j];
                P[j]=P[k];
                P[k]=temp;

            }

        }

    }

    for(int i = 0; i<N-3; i++)
    {
        //gonderdigimiz 2 nokta arasindaki uzaklik
        I = uzaklik(P[i],P[i+3]);
        int n = int(I);
        //4 nokta gonderiyoruz qubic oldugu icin
        b_spline_algorithm(P[i],P[i+1],P[i+2],P[i+3],n);
        //line(splinex[splineUzunluk-1]+500,spliney[splineUzunluk-1]+500,P[N-1].X,P[N-1].Y);

        //spline listesi icinde dolasiyoruz ve 2 nokta arasinda line cizdirerek b-spline egrisini olusturuyoruz.
        for(int j=1; j<splineUzunluk; j++)
        {
            int x = int(splinex[j]);
            int y = int(spliney[j]);
            int x1 = int(splinex[j-1]);
            int y1 = int(spliney[j-1]);

            setcolor(YELLOW);

            line(x+500,500-y,500+x1,500-y1);

        }
    }

    line(splinex[0]+500,500-spliney[0],P[0].X*olcek+500,500-P[0].Y*olcek);
    line(splinex[splineUzunluk-1]+500,500-spliney[splineUzunluk-1],P[N-1].X*olcek+500,500-P[N-1].Y*olcek);


}

int main()
{

    //Dosya islemleri-------------------------
    FILE *f;
    int i=0 ;
    struct Nokta points [1000];
    if((f = fopen("numbers.txt", "r")) == NULL)
    {
        printf("Dosya okumada bir sorun olustu!");
        exit(1);
    }
    fscanf(f, "%d", &N);
    for(i=0; i<N; i++)
    {
        fscanf(f, "%lf %lf",&points[i].X,&points[i].Y);
    }
    //dosya islemleri--------------------------
    struct Cember mec = minimum_cember(points,N);
    koordinat_ciz();
    noktalari_goster(points);
    printf("\nX=%lf Y=%lf R=%lf\n",mec.C.X,mec.C.Y,mec.R);

    int ix = mec.C.X*olcek,yy= mec.C.Y*olcek,rr= mec.R*olcek;

    char strM[10];
    sprintf(strM,"M = (%.3lf , %.3lf)",mec.C.X,mec.C.Y);
    char strR[10];
    sprintf(strR,"R = %3.lf",mec.R);
    setcolor(GREEN);
    outtextxy(ix+500,500-yy,"M");

    setcolor(WHITE);
    outtextxy(900,15,strM);
    outtextxy(900,35,strR);
    circle(ix+500,500-yy,1);

    setcolor(GREEN);
    circle(ix+500,-yy+500,rr);

    b_spline(points);
    noktalari_goster(points);


    getch();
    closegraph();
    return 0;
}
