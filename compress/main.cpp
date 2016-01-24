/*
 *compress trajectory
 *author: BWChen
 *date: 2015-12-20
 */
#include <iostream>
#include <fstream>
#include <limits.h>
#include <vector>
#include <stack>
#include <list>
#include <math.h>
#include <stdlib.h>
#include <iomanip>
#include <string>
#define MAXLENGTH 5000
using namespace std;

double pointList[MAXLENGTH][2];
double delta = 30;
int length = 0;
void split(string &s, string &delim, vector<string> &ret);
int readData(string fileName);
double distance(int s, int e, int i);
list<int> DouglasPeucker(int s, int e);
void writeFile(list<int> compressList, string fileName);
double Rad(double d);
double Geodist(double lat1, double lon1, double lat2, double lon2);

void split(string &s, string &delim, vector<string> &ret)
{
    size_t last = 0;
    size_t index = s.find_first_of(delim, last);
    while(index != string::npos)
    {
        ret.push_back(s.substr(last, index - last));
        last = index + 1;
        index = s.find_first_of(delim, last);
    }
    if(index - last > 0)
    {
        ret.push_back(s.substr(last, index - last));
    }
}

int readData(char* fileName)
{
    ifstream in(fileName);
    string line;
    string delim = ",";
    int i = 0;
    if(in)
    {
        while(getline(in, line))
        {
            vector<string> ret;
            split(line, delim, ret);
            double latitude = atof(ret[0].c_str());
            double longitude = atof(ret[1].c_str());
            pointList[i][0] = latitude;
            pointList[i][1] = longitude;
            i++;
        }
    }
    else
    {
        return 0;
    }
    length = i;
    return 1;
}

double Geodist(double lat1, double lon1, double lat2, double lon2)
{
    double radLat1 = Rad(lat1);
    double radLat2 = Rad(lat2);
    double delta_lon = Rad(lon2 - lon1);
    double top_1 = cos(radLat2) * sin(delta_lon);
    double top_2 = cos(radLat1) * sin(radLat2) - sin(radLat1) * cos(radLat2) * cos(delta_lon);
    double top = sqrt(top_1 * top_1 + top_2 * top_2);
    double bottom = sin(radLat1) * sin(radLat2) + cos(radLat1) * cos(radLat2) * cos(delta_lon);
    double delta_sigma = atan2(top, bottom);
    double distance = delta_sigma * 6378137.0;
    return distance;
}
double Rad(double d)
{
    return d * M_PI / 180.0;
}

double distance(int s, int e, int i)
{
    double lat_start = pointList[s][0];
    double lon_start = pointList[s][1];
    double lat_end = pointList[e][0];
    double lon_end = pointList[e][1];
    double lat_i = pointList[i][0];
    double lon_i = pointList[i][1];

    double a = Geodist(lat_start, lon_start, lat_i, lon_i);
    double b = Geodist(lat_i, lon_i, lat_end, lon_end);
    double c = Geodist(lat_start, lon_start, lat_end, lon_end);

    double p = (a + b + c) / 2;
    double area = sqrt(p * (p - a) * (p - b) * (p - c));
    double distance = 2 * area / c;
    return distance;
}

list<int> DouglasPeucker(int s, int e)
{
    list<int> indecies;
    list<int>::iterator iter;
    double dmax = 0;
    double temp_d = 0;
    int idx = 0;
    int i = 0;

    for(i = s + 1; i < e; i++)
    {
        temp_d = distance(s, e, i);
        if(temp_d > dmax)
        {
            dmax = temp_d;
            idx = i;
        }
    }
    if(dmax > delta)
    {
        list<int> L1 = DouglasPeucker(s, idx);
        list<int> L2 = DouglasPeucker(idx, e);
        for(iter = L1.begin(); iter != L1.end(); iter++)
        {
            int index = *iter;
            indecies.push_back(index);
        }
        indecies.pop_back();
        for(iter = L2.begin(); iter != L2.end(); iter++)
        {
            int index = *iter;
            indecies.push_back(index);
        }
    }
    else
    {
        indecies.push_back(s);
        indecies.push_back(e);
    }
    return indecies;
}

void writeFile(list<int> compressList, char* fileName)
{
    int i = 0;
    ofstream fout(fileName);
    double latitude = 0;
    double longitude = 0;
    list<int>::iterator iter;
    for(iter = compressList.begin(); iter != compressList.end(); iter++)
    {
        int idx = *iter;
        latitude = pointList[idx][0];
        longitude = pointList[idx][1];
        fout<<setprecision(8)<<idx<<","<<latitude<<","<<longitude<<endl;
        i++;
    }
}

int main()
{
    char* beforeData = "Data.txt";
    char* afterData = "compressList.txt";
    readData(beforeData);
    list<int> compressList = DouglasPeucker(0, 3150);
    compressList.pop_back();
    double rate = 1 - (double)compressList.size() / (double)length;
    writeFile(compressList, afterData);
    cout<<"The original data file: "<<beforeData<<endl;
    cout<<"The compressed data file: "<<afterData<<endl;
    cout<<"\t-"<<length<<" data points before compressing"<<endl;
    cout<<"\t-"<<compressList.size()<<" data points after compressing"<<endl;
    cout<<"\t-"<<"Compression rate:"<<rate<<endl;
    return 0;
}
