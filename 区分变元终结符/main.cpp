#include<iostream>
#include<string>
#include<map>
#include<vector>
#include<set>
using namespace std;
#define FILENAME "wenfa.txt"
#define MAXROW 100
#define EPSILON "ε"
string var;//变元
map<string,int> var_list;//变元表
map<string,int> ter_list;//终结符表
map<int,string> re_var_list;//变元表
map<int,string> re_ter_list;//终结符表
map<string,int>::iterator it;
class element
{
public:
    bool isVar;
    int index;
   /* string& toString() const
    {
		if(index<re_var_list.size()||index<re_ter_list.size())
            return isVar?re_var_list[index]:re_ter_list[index];
		else
			return *(new string("null"));
    }*/
	string& toString() const
    {
        //Judge whether the index is of meaning.
        if((isVar&&re_var_list.find(index)!=re_var_list.end())||(!isVar&&re_ter_list.find(index)!=re_ter_list.end()))
            return isVar?re_var_list[index]:re_ter_list[index];
        else
            return *(new string("null"));
    }

    element(bool isVar,int index)
    {
        this->isVar=isVar;
        this->index=index;
    }
    element(const element& elem)
    {
        isVar=elem.isVar;
        index=elem.index;
    }
    element()
    {}
    bool operator<(const element& elem)const
    {
        return this->index<elem.index;
    }
    bool operator>(const element& elem)const
    {
        return this->index>elem.index;
    }
};

class production
{
public:
    element l_part;
	bool isWithEPSILON;
    vector<element> r_part;
    int r_part_size;
    production()
    {
        r_part_size=0;
		isWithEPSILON=false;
    }
    production(const production& produc)
    {
		isWithEPSILON=produc.isWithEPSILON;	
        set_l_part(produc.l_part.isVar,produc.l_part.index);
        vector<element>::iterator it;
        //for(it=produc.r_part.begin();it<produc.r_part.end();it++)
          //  insert_elem(*it);
		for(int i=0;i<produc.r_part.size();i++)
			insert_elem(produc.r_part[i]);
		r_part_size=produc.r_part_size;
		
    }
    void set_l_part(bool isVar,int index)
    {
        l_part.isVar=isVar;
        l_part.index=index;
    }
    void insert_elem(const element& elem)
    {
        r_part.push_back(elem);
        r_part_size++;
		if(elem.toString().compare(EPSILON)==0)
			isWithEPSILON=true;
    }
    string& toString()
    {
        string* produc=new string();
        (*produc)+=l_part.toString();
        (*produc)+=string("->");
        vector<element>::iterator it;
        for(it=r_part.begin();it<r_part.end();it++)
        {
            (*produc)+=it->toString();
            if(it!=r_part.end()-1)
                (*produc)+=string(" ");
        }
        return *(produc);

    }


};
vector<production> produc_set;//产生式集合
class FIRST
{
private:
    map<string,int> FIRST_map;
    vector<set<element> > FIRST_sets;
    int i_pointer;
public:
    FIRST()
    {
        i_pointer=0;
    }
   
    //Return: successful or not
    bool insert(const string& x,const element& elem)
    {
        bool result=0;

        //Judge whether x exists in var_list or ter_list.
        if(var_list.find(x)==var_list.end()&&ter_list.find(x)==ter_list.end())
            return 0;

        map<string,int>::iterator it;

        //FISRT(x) has not been established.
        if(FIRST_map.find(x)==FIRST_map.end())
        {
            FIRST_map.insert(pair<string,int>(x,i_pointer));
            //Establish a FIRST set with nothing.
            FIRST_sets.push_back(set<element>());
            i_pointer++;
        }


        it=FIRST_map.find(x);
        //Judge whether elem exists in FISRT(x).
        if(FIRST_sets[it->second].find(elem)!=FIRST_sets[it->second].end())
            result=0;
        else
        {
            FIRST_sets[it->second].insert(elem);
            result=1;
        }
        return result;

    }

   set<element>& find(string x)
    {
        set<element>* a=new set<element>;

         //Judge whether x exists in var_list or ter_list.
        if(var_list.find(x)==var_list.end()&&ter_list.find(x)==ter_list.end())
            return *a;

        //If x is a terminative, return {x}.
        if(ter_list.find(x)!=ter_list.end())
        {
            a->insert(element(0,ter_list.find(x)->second));
            return *a;
        }
        else
        {
            //FISRT(x) has not been established.
            if(FIRST_map.find(x)==FIRST_map.end())
            {
                FIRST_map.insert(pair<string,int>(x,i_pointer));
                //Establish a FIRST set with nothing.
                FIRST_sets.push_back(set<element>());
                i_pointer++;
            }

            map<string,int>::iterator it;
            it=FIRST_map.find(x);
            return FIRST_sets[it->second];
        }



    }
	
	void print()
	{
		set<element>::iterator set_it;
		for(it=FIRST_map.begin();it!=FIRST_map.end();it++)
		{
			cout<<it->first<<":";
			for(set_it=FIRST_sets[it->second].begin();set_it!=FIRST_sets[it->second].end();set_it++)
				cout<<set_it->toString()<<" ";
			cout<<endl;
		}
	}
}first_sets;
void split(std::string& s, std::string& delim,std::vector< std::string >* ret);
void init_first_sets(map<string,int>& var_list);
	
void main()
{
	int i=1;
	string s=" ";
	string str;
	string ter_var;//产生式右边的串
	vector<string> var_ter_vec;//产生式右部的划分
	
	char cstr[MAXROW];
	int pos;
	FILE *f;
	if((f=fopen(FILENAME,"r"))==NULL)
	{
		cout<<"open error"<<endl;
		exit(0);
	}
	while(!feof(f))
	{
	fgets(cstr,MAXROW,f);
	str=cstr;
	pos=str.find('→');
	if(pos>0)
	{
		var=str.substr(0,pos-1);
		if((var_list.find(var))==var_list.end())//没找到
		{
			var_list.insert(pair<string,int>(var,i));//插入变元表
			re_var_list.insert(pair<int,string>(i,var));
			i++;
		}
	}
	}
	fclose(f);

	if((f=fopen(FILENAME,"r"))==NULL)
	{
		cout<<"open error"<<endl;
		exit(0);
	}
	i=1;
	while(!feof(f))
	{
	fgets(cstr,MAXROW,f);
	str=cstr;
	if(str[str.length()-1]=='\n')
		str.resize(str.size()-1);
	pos=str.find('→');
	if(pos>0)
	{
		production product;
		product.set_l_part(true,var_list.find(str.substr(0,pos-1))->second);
		//ter_var=str.substr(pos+1,str.length()-pos-2);//产生式右部
		ter_var=str.substr(pos+1,str.length()-pos-1);//产生式右部
		split(ter_var,s,&var_ter_vec);
		for(int j=0;j<var_ter_vec.size();j++)
		{
			if((var_list.find(var_ter_vec[j]))==var_list.end())//不是变元，即为终结符
			{

				if(ter_list.find(var_ter_vec[j])==ter_list.end())
				{
				   product.insert_elem(element(false,i));//产生式插入右部终结符
				   ter_list.insert(pair<string,int>(var_ter_vec[j],i));//插入终结符表				   
				   re_ter_list[i]=var_ter_vec[j];//re_ter_list.insert(pair<int,string>(i,var_ter_vec[j]));
				   i++;
				}
				else
					product.insert_elem(element(false,ter_list.find(var_ter_vec[j])->second));//产生式插入右部终结符
			}
			else//变元
			{
				
				product.insert_elem(element(true,(var_list.find(var_ter_vec[j]))->second));//产生式插入右部变元
			}
		}	
		produc_set.push_back(product);//构造produc_set
	}    
	}
	fclose(f);
	init_first_sets(var_list);
	first_sets.print();
	/*cout<<"变元表"<<endl;
	for(it=var_list.begin();it!=var_list.end();it++)
	{
		cout<<it->first<<"===="<<it->second<<endl;
	}
	cout<<"终结符表"<<endl;
	for(it=ter_list.begin();it!=ter_list.end();it++)
	{
		cout<<it->first<<"===="<<it->second<<endl;
	}
	*/
}
void split(std::string& s, std::string& delim,std::vector< std::string >* ret)  
{  //将s按照delim划分存到ret中
    size_t last = 0;  
    size_t index=s.find_first_of(delim,last);  
	ret->clear();
    while (index!=std::string::npos)  
    {  
        ret->push_back(s.substr(last,index-last));  
        last=index+1;  
        index=s.find_first_of(delim,last);  
    }  
    if (index-last>0)  
    {  
        ret->push_back(s.substr(last,index-last));  
    }  
}  

void init_first_sets(map<string,int>& var_list)
{//求所有变元的first集
	bool isChanged=false;
	map<string,int>::iterator it;
	set<element>::iterator set_it;

	while(1)
	{
		isChanged =false;
		for(it=var_list.begin();it!=var_list.end();it++)
		{//遍历变元表
			 for(int i=0;i<produc_set.size();i++)
			 {//遍历产生式集合
				 if((it->first.compare(produc_set[i].l_part.toString()))==0)//找到该变元X的产生式
				 {
					if(!produc_set[i].r_part[0].isVar)//X->a,注意空也是终结符
						isChanged|=first_sets.insert(it->first,produc_set[i].r_part[0]);//将a插入first(X)						
					else//X->Y
					{
						for(int k=0;k<produc_set[i].r_part.size();k++)
						{//对Y1-YN遍历
							if(!(var_list.find(produc_set[i].r_part[k].toString())==var_list.end()))
							{//Yk是变元							
								set<element> first_yk=first_sets.find(produc_set[i].r_part[k].toString());//first(Yk)
								for(set_it=first_yk.begin();set_it!=first_yk.end();set_it++)//将first(Yk)中所有非空元素加入first(X)
									if((*set_it).toString().compare(EPSILON)!=0)	
										isChanged|=first_sets.insert(it->first,*set_it);
								if(first_sets.find(produc_set[i].r_part[k].toString()).find(element(false,ter_list.find(EPSILON)->second))!=(first_sets.find(produc_set[i].r_part[k].toString())).end())
								{//如果first(Yk)有空
									if(k==produc_set[i].r_part.size()-1)
									{									
										isChanged|=first_sets.insert(it->first,element(false,ter_list.find(EPSILON)->second));//插入空
										break;
									}
								}
								else
									break;
							}
							
						}
					}
				 }
			 }
		}
		if(isChanged==false)
			break;
	}

}