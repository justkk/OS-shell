#include<stdio.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/utsname.h>
#include<wait.h>
#include<signal.h>


#include<sys/types.h>
#include<sys/ptrace.h>
#include<sys/stat.h>


int waitingpid=0;
int fgpid=0;
int shell_terminal;
int shell_pgid;
struct history{
	pid_t pid;
	pid_t ppid;
	int over;
	char pname[100]; // contains the command name
	char pname1[100]; // contains the command name
	int status;// to get the status of the job whether it is completed or stopped/ terminated
};
int presentparentid=0;
int sigchi=0;
int majorid;
int killid=0;
typedef struct history history;
history hist[100];
int histlen=1;
int cher=0;
char fgh[100];
char command2[100];
char ref2[100];
char prompt[100]={'\0'};
void  signal_handler(int sig)// signal handler for the parent
{
	if(sig == SIGINT)// sigint interupt handiling
	{
		int st;
		write(1,"\nrecieved SIGINT\n",17);
		int z = waitpid(waitingpid,&st,WNOHANG);
		int i=1;
		if(waitingpid>0)
		{
		while(i<histlen)// updating the all the process status that recieved the sigint and terminating it
		{
			if(hist[i].pid==waitingpid)
			{
				hist[i].status=-1;
				break;
			}
			i++;
		}
		}
		else
		{
			write(1,prompt,strlen(prompt));
		}
		cher=1;
	}
	if(sig == 3)// sigquit handler this neglects the interput to running terminal
	{
		write(1,"\nrecieved SIGQUIT\n",18);
		write(1,prompt,strlen(prompt));
		cher=1;
	}
	if(sig == SIGTSTP)// sig stop signal this is to stop the current running process in the fore ground and sent it to back
	{
		write(1,"\nrecieved 19\n",13);
		if(waitingpid==0)
		{
			write(1,prompt,strlen(prompt));
		}
		else
		{
			int i=1;
			int yp=1;
			while(i<histlen)// updating the status of all the process
			{
				if(hist[i].pid==waitingpid)
				{
					yp=0;
					hist[i].status=0;
					break;
				}
				i++;
			}
			if(yp==1)
			{
				hist[histlen].pid=waitingpid;
				hist[histlen].ppid=getpid();
				strcpy(hist[histlen].pname,command2);
				strcpy(hist[histlen].pname1,ref2);
				hist[histlen].status=0;
				histlen++;
			}
			char r[]=" PROCESS STOPPED AND SENT TO BACKGROUND\n";
			write(1,hist[i].pname,strlen(hist[i].pname));// printing to the console success
			write(1,r,strlen(r));
			kill(waitingpid,SIGSTOP);// sending stop signal to the child
		}
		cher=1;
	}
	else// basically handles sigcj=hild intreput
	{
		int st;
		int z = waitpid(-1,&st,WNOHANG);
		int i;
		if(z<=0 && killid > 0)
		{
			z=killid;
			killid=0;
		}
		if(waitingpid==0)
		{

			for(i = 1; i< histlen; i++)// updating the status of the running child process
			{	
				if (hist[i].pid == z)
				{
					if(WIFEXITED(st))
					{
						sigchi=1;
						hist[i].status=-1;
					}
					else if(WIFSTOPPED(st))
					{
						hist[i].status=0;
					}
					strcpy(fgh,"\n");
					char ee[5];
					sprintf(ee,"%d",hist[i].pid);
					strcat(fgh,ee);
					strcat(fgh," ");
					strcat(fgh,hist[i].pname);
					strcat(fgh," successfully terminated \n");// successfull display
					break;
				}
			}
		}

	}

}

/*


 */

int main()
{
	/*	shell_terminal=STDERR_FILENO;
		}*/

	int index=0;
	majorid=getpid();
	char mainname[100];
	getcwd(mainname,100);
	int start=0;
	char changingdir[100];
	strcpy(changingdir,mainname);
// handler functions
	if(signal(SIGCHLD,signal_handler)==SIG_ERR)
	{
		write(1,"\ncannot catch signals\n",22);
	}
	if(signal(SIGTSTP,signal_handler)==SIG_ERR)
	{
		write(1,"\ncannot catch signals\n",22);
	}


	if(signal(SIGINT,signal_handler)==SIG_ERR)
	{
		write(1,"\ncannot catch signals\n",22);
	}
	if(signal(SIGQUIT,signal_handler)==SIG_ERR)
	{
		write(1,"\ncannot catch signals\n",22);
	}/*
	    }*/
	signal(SIGTTOU,SIG_IGN);// ingnoring signals so that no interupt occurs while changing the terminal working gp id
	signal(SIGHUP,SIG_IGN);
	signal(SIGTTIN,SIG_IGN);
	while(1)
	{
		// splitting for the command
		int redirectout=0;
		int redirectin=0;
		int fdin[2];
		int fdout[2];
		pipe(fdin);
		pipe(fdout);
		waitingpid=0;
		int asd=1;
		/*	while(asd<histlen)
		}*/
		char actualname[100];
		getcwd(actualname,100);
		char nowdir[100]={'\0'};
		int y=strlen(mainname);
		int yy=y;
		int z=strlen(actualname);
		actualname[z]='\0';
		char point;
		int hh=0;
		while(y<z)
		{
			nowdir[hh]=actualname[y];
			y++;
			hh++;
		}
		int chery=0;
		int yui=0;
		while(yui < yy)
		{
			if(mainname[yui]==actualname[yui])
			{
			}
			else
			{
				chery=1;
				break;
			}
			yui++;
		}
		char *name;
		name=getlogin();
		struct utsname username;
		uname(&username);
		char f[100]={'\0'};
		strcat(f,"<");
		strcat(f,name);
		strcat(f,"@");
		strcat(f,username.nodename);
		strcat(f,":");
		// concatinating various string for the prompt;
		if( chery==0)
		{
			strcat(f,"~");
			strcat(f,nowdir);
		}
		else if(chery!=0)
		{
			strcat(f,actualname);

		}
		strcat(f," $");
		strcat(f,">");
		strcpy(prompt,f);
		write(1,prompt,strlen(prompt));


		char commandfull[100];
		fgets(commandfull,100,stdin);// scanning command
		commandfull[strlen(commandfull)-1]='\0';// jast value keeping it as a null as it contains '\n'
		if(strlen(commandfull)!=0)
		{


			char tap[100];
			char ink[100][100];
			strcpy(tap,commandfull);
			char *tin;
			int yopped=0;
			tin=strtok(tap,"|");// spliting with pipe using strtok
			while(tin!=NULL)
			{
				strcpy(ink[yopped],tin);
				tin=strtok(NULL,"|");
				yopped++;
				if(tin!=NULL)
				{

				}
			}
			int fdpipe[2*yopped];
			int yip=0;
			while(yip<2*yopped)
			{
				pipe(fdpipe+yip);
				yip=yip+2;
			}
			int oddeven=0;
			int yop=0;
			while(yop<yopped)
			{
				char commandpipe[100];
				strcpy(commandpipe,ink[yop]);
				yop++;
				if(redirectout==1)
				{
					redirectin=1;
				}
				else
				{
					redirectin=0;
				}
				if(yop!=yopped)
				{
					redirectout=1;
				}
				else
				{
					redirectout=0;
				}
				if(strlen(commandpipe)!=0)
				{
					char command[100];
					char commandforwork[100];
					strcpy(commandforwork,commandpipe);
					char* splittingbyarrow;
					splittingbyarrow=strtok(commandpipe,"<>");
					strcpy(command,splittingbyarrow);
					char inputfile[100];
						int yp=0;
					char outputfile[100];
					int inputfiledesc=0;
					int outputfiledesc=1;
					splittingbyarrow=strtok(NULL,"<>");// spliting using < > and storing input and output pointers
					if(splittingbyarrow!=NULL)
					{
						char unified[100];
						char unified1[100];
						char *tt;
						tt=NULL;
						strcpy(unified,commandforwork);
						strcpy(unified1,commandforwork);
						char *i1=strtok(commandforwork,"<");
						i1=strtok(NULL,"<");
						if(i1!=NULL)
						{
							tt=strtok(i1,">");
						}
						if(tt!=NULL)
						{
							i1=tt;
						}
						char *o1=strtok(unified,">");
						o1=strtok(NULL,">");
						char *o2=strstr(unified1,">>");
						tt=NULL;
						if(o1!=NULL)
						{
							tt=strtok(o1,"<");
						}
						if(tt!=NULL)
						{
							o1=tt;
						}
						if(i1!=NULL)
						{
							i1=strtok(i1," ");
						}
						int kil=0;
						if(o1!=NULL)
						{
							o1=strtok(o1," ");
						}
						if(i1!=NULL)
						{
							yp=1;
							inputfiledesc=open(i1,O_RDONLY);
						}
						if(o1!=NULL)
						{
							if(o2==NULL)
							{
								outputfiledesc=open(o1,O_WRONLY|O_CREAT|O_TRUNC,0777);
							}
							else
							{
								outputfiledesc=open(o1,O_WRONLY|O_CREAT|O_APPEND,0777);
							}

						//	if(kil==1 && outputfiledesc!=-1)
						//	{
						//		lseek(outputfiledesc,-1,SEEK_END);
						//	}
						}

					}
					if(redirectin==1)
					{
						if(oddeven%2==0)
						{
						}
						else
						{
						}
					}
					else
					{
					}
					if(redirectout==1)
					{
						if(oddeven%2==0)
						{
						}
						else
						{
						}

					}
					else
					{
					}
					if(1)
					{	
						// runnig the command
						char command1[100];
						char command2[100];
						strcpy(command1,command);
						strcpy(command2,command);
						char* cnam;
						char* argval;
						cnam=strtok(command1," \t");
						argval=strtok(NULL," \t");
						int forking=0;
						char *arg[30];
						char *split;
						char ref[100];
						split=strtok(command," \t");
						int i=0;
						int check=0;
						while(split!=NULL)
						{
							if(i==0)
							{
								strcpy(ref,split);
							}
							arg[i]=split;
							split=strtok(NULL," \t");
							i++;
						}
						arg[i]=NULL;
						if(strcmp(arg[i-1],"&")==0)
						{
							arg[i-1]=NULL;
							forking=1;
						}
						int h=i-1;
						char cd[100]={'\0'};// printing in child process array 
						char jobs[100]={'\0'};
						char kjob[100]={'\0'};
						char overkill[100]={'\0'};
						char fg[100]={'\0'};
						if(strcmp(ref,"cd")==0)// parent process ruunnig 
						{
							if(argval==NULL || (strcmp(arg[1],"~")==0 && arg[2]==NULL))
							{
								chdir(mainname);
							}
							else
							{
								int r=chdir(argval);
								if(r==-1)
								{
									char tt[]="folder name dose'nt exists \n";
									strcat(cd,tt);
								}
							}
						}
						else if(strcmp(ref,"kjob")==0)// kjob running in parent process
						{
							if(redirectin==1)
							{
								char argum[100];
								char *a1=NULL;
								char *a2=NULL;
								read(fdpipe[2*(yop-1)],argum,100);
								a1=strtok(argum," \n");
								arg[1]=a1;
								if(a1!=NULL)
								{
									a2=strtok(NULL," \n");
									arg[2]=a2;
								}
							}
							if(inputfiledesc>0)
							{
								char argum[100];
								char *a1=NULL;
								char *a2=NULL;
								read(inputfiledesc,argum,100);
								a1=strtok(argum," \n");
								arg[1]=a1;
								if(a1!=NULL)
								{
									a2=strtok(NULL," \n");
									arg[2]=a2;
								}
							}
							if(arg[1]!=NULL && arg[2]!=NULL)
							{
								int v=atoi(arg[1]);
								int vv=atoi(arg[2]);
								if(vv==9)
								{
									killid=v;
									waitingpid=0;
								}
								waitingpid=-1;
								int i=1;
								int jj=0;
								while(i<histlen)
								{
									if(hist[i].status!=-1)
									{
										jj++;
										if(jj==v)
										{
											int u=kill(hist[i].pid,vv);

											if(vv==9)// updating

											{
												hist[i].status=-1;
												char r[]=" PROCESS TERMINATED\n";
									strcat(kjob,hist[i].pname);
									strcat(kjob,r);
											}
											else if(vv==19)// updating
											{
												hist[i].status=0;
												char r[]=" PROCESS STOPPED AND SENT TO BACKGROUND\n";
									strcat(kjob,hist[i].pname);
									strcat(kjob,r);
											}
											break;
										}
									}
									i++;
								}
							}
							else
							{
								char gg[]="WRONG COMMAND\n";
								strcat(kjob,gg);
							}


						}
						else if(strcmp(ref,"overkill")==0)// over kill in parent process
						{
							int i=1;
							while(i<histlen)
							{
								if(hist[i].status==1 || hist[i].status==0)// updating 
								{
									int x;
									kill(hist[i].pid,9);
									waitpid(hist[i].pid,&x,WUNTRACED);
									hist[i].status=-1;
								}
								i++;
							}

						}
						else if(strcmp(ref,"fg")==0)// sending into background
						{
							if(redirectin==1)
							{
								char argum[100];
								char *a1=NULL;
								char *a2=NULL;
								read(fdpipe[2*(yop-1)],argum,100);
								a1=strtok(argum," \n");
								arg[1]=a1;
							}
							if(inputfiledesc!=0)
							{
								char argum[100];
								char *a1=NULL;
								read(inputfiledesc,argum,100);
								a1=strtok(argum," \n");
								arg[1]=a1;
							}
							if(arg[1]!=NULL && arg[2]==NULL)
							{
								int pval=atoi(arg[1]);
								int i=1;
								int gq=1;
								int upid=-1;
								while(i<histlen)
								{
									if(hist[i].status==0 || hist[i].status==1)
									{
										if(gq==pval)
										{
											upid=hist[i].pid;
											break;
										}
										else
										{
											gq++;
										}

									}
									i++;
								}
								if(upid==-1)
								{
									char tt[]="ERROR\n";
									write(2,tt,strlen(tt));
								}
								else
								{
									waitingpid=upid;
									hist[i].status=-1;
									if(getpgid(upid)==getpid())
									{
										kill(upid,SIGCONT);
										int yq;
										waitpid(upid,&yq,WUNTRACED);
										if(WIFSTOPPED(yq))
										{
											char r[]=" PROCESS STOPPED AND SENT TO BACKGROUND\n";
										}
										else if(WIFEXITED(yq))
										{
											char r[]=" PROCESS TERMINATED\n";
											strcat(fg,hist[i].pname);
											strcat(fg,r);
										}

									}
									else
									{
										int head=getpgid(getpid());
										setpgid(getpid(),upid);
										tcsetpgrp(STDIN_FILENO,upid);
										kill(upid,SIGCONT);
										int yq;
										waitpid(upid,&yq,WUNTRACED);
										if(WIFSTOPPED(yq))
										{
											char r[]=" PROCESS STOPPED AND SENT TO BACKGROUND\n";
										}
										else if(WIFEXITED(yq))
										{
											char r[]=" PROCESS TERMINATED\n";
											strcat(fg,hist[i].pname);
											strcat(fg,r);
										}
										setpgid(getpid(),head);
										tcsetpgrp(STDIN_FILENO,getpid());
									}

								}

							}
						}		
						else if(strcmp(ref,"jobs")==0)// jobs command
						{
							int i=1;
							int kaku=1;
							while(i<histlen)
							{
								if(hist[i].status==1 || hist[i].status == 0)//checking the status
								{
									char num[5];
									char num1[5];
									char indexc[5];
									sprintf(num,"%d",hist[i].pid);
									sprintf(num1,"%d",getpgid(hist[i].pid));
									sprintf(indexc,"%d",kaku);
										strcat(jobs,"(");
										strcat(jobs,indexc);
										strcat(jobs,")");
										strcat(jobs," ");
									if(hist[i].status==0)
									{
										strcat(jobs,"Stopped");
									}
									if(hist[i].status==1)
									{
										strcat(jobs,"Running");
									}
										strcat(jobs,hist[i].pname1);
										strcat(jobs," [ ");
										strcat(jobs,num);
										strcat(jobs," ");
										strcat(jobs,num1);
										strcat(jobs," ]\n");
									kaku++;
								}
								i++;
							}

						}
						else if (strcmp(ref,"quit")==0)// quit process
						{
							_exit(0);


						}
						if(1)
						{
							pid_t pid;
							pid=fork();
							if(forking==0 && pid!=0)// creating arary link
							{
								waitingpid=pid;
								strcpy(command2,command);
								strcpy(ref2,ref);
								hist[histlen].pid=pid;
								hist[histlen].ppid=getpid();
								strcpy(hist[histlen].pname,command2);
								strcpy(hist[histlen].pname1,ref);
								hist[histlen].status=1;
								histlen++;
							}
							if (forking==1 && pid!=0)//diff array link for child process
							{
								waitingpid=0;
								hist[histlen].pid=pid;
								hist[histlen].ppid=getpid();
								strcpy(hist[histlen].pname,command2);
								strcpy(hist[histlen].pname1,ref);
								hist[histlen].status=1;
								histlen++;

							}
							if(pid==0)
							{
								if(redirectin==1 && (strcmp(ref,"pinfo")!=0))
								{
									dup2(fdpipe[2*(yop-1)],0);// getting input
									close(fdpipe[2*(yop-1)]);// closing for the save copy
								}
								else
								{
								}
								if(redirectout==1 && yop<yopped)
								{
									dup2(fdpipe[2*(yop-1)+3],1);// writing into the output
									/*if(oddeven%2==0)
									}*/

								}
								else
								{
								}
								if(inputfiledesc==-1 && yp==1)
								{
									char tt[]="ERROR\n";
									write(2,tt,strlen(tt));
									_exit(0);
								}
								if(inputfiledesc>0)
								{
									dup2(inputfiledesc,0);// input pointers
								}
								if(outputfiledesc>1)
								{
									dup2(outputfiledesc,1);// output pointers
								}
								signal(SIGTTOU,SIG_DFL);// setting default signals
								signal(SIGTTIN,SIG_DFL);
								if(forking==0)
								{

								}
								if(forking==1 && redirectout==1)
								{
									char tt[]="ERROR\n";
									write(2,tt,strlen(tt));
									_exit(0);
								}
								else if (forking==1)
								{
									setpgid(getpid(),getpid());// setting pgid diff for child
									int fd;
								}
								if(strcmp(ref,"jobs")==0)
								{
									write(1,jobs,strlen(jobs));// writing output of jobs
									_exit(0);
								}
								if(strcmp(ref,"kjob")==0)
								{
									write(1,kjob,strlen(kjob));// writing output of kjob
									_exit(0);
								}
								if(strcmp(ref,"fg")==0)
								{
									write(1,fg,strlen(fg));// writing fg of the job
									_exit(0);
								}
								if(strcmp(ref,"overkill")==0)
								{
									write(1,overkill,strlen(overkill));// overkill output
									_exit(0);
								}
								if(strcmp(ref,"cd")==0)
								{
									write(1,cd,strlen(cd));// out put of cd
									_exit(0);
								}
								if(strcmp(ref,"./a.out")==0)// ./a.out command implementation
								{
									execlp("./a.out","./a.out",arg,NULL);
								}
								else if(strcmp(ref,"pinfo")==0)// pinfo child process
								{
									if(redirectin==1)
									{
										char argum[100];
										char *a1=NULL;
										char *a2=NULL;
										read(fdpipe[2*(yop-1)],argum,100);
										a1=strtok(argum," \n");
										arg[1]=a1;
										arg[2]=NULL;
									}
									if(inputfiledesc!=0)
									{
										char argum[100];
										char *a1=NULL;
										read(inputfiledesc,argum,100);
										a1=strtok(argum," \n");
										arg[1]=a1;
										arg[2]=NULL;
									}
									if(arg[1]==NULL)
									{
										int aInt = getpid();
										char str[15];
										sprintf(str, "%d", aInt);
										arg[1]=str;
										arg[2]=NULL;
									}

									int counting=1;
									if(arg[2]==NULL)
									{

										int pp=atoi(arg[counting]);// getting the status of from the /proc/id
										char qq[100]="/proc/";
										char id[5];
										sprintf(id,"%d",pp);
										strcat(qq,id);
										char tt[100]="/status";
										char tata[100];
										strcpy(tata,qq);
										strcat(tata,"/exe");
										strcat(qq,tt);
										FILE *des=fopen(qq,"r");
										if(des!=NULL)
										{
											char forname[100];
											fgets(forname,100,des);
											char *oo;
											char *vv;
											char *bb;
											char pnames[100],pready[100];
											oo=strtok(forname," \t");
											bb=strtok(NULL," \t");
											strcpy(pnames,bb);
											fgets(forname,100,des);
											oo=strtok(forname," \t");
											bb=strtok(NULL," \t");
											bb=strtok(NULL," \t");
											strcpy(pready,bb);
											fgets(forname,100,des);
											fgets(forname,100,des);
											fgets(forname,100,des);
											fgets(forname,100,des);
											fgets(forname,100,des);
											fgets(forname,100,des);
											fgets(forname,100,des);
											fgets(forname,100,des);
											fgets(forname,100,des);
											fgets(forname,100,des);
											oo=strtok(forname," \t");
											if(oo[0]=='S')
											{
												strcpy(bb,"0");
											}
											else
											{

												bb=strtok(NULL," \t");
											}
											char ans[1000]={'\0'};
											int yy=readlink(tata,ans,100);
											write(1,"EXECUTABLEPATH : ",17);
											if(yy==-1)
											{

												write(2,"PERMISSION DENIED\n",18);
											}
											write(1,ans,strlen(ans));
											write(1,"\n",1);
											char sizes[10];
											strcpy(sizes,bb);
											char parcel[100];
											strcpy(parcel,"Name -> ");
											strcat(parcel,pnames);
											strcat(parcel,"\nState -> ");
											strcat(parcel,pready);
											strcat(parcel,"\nSize -> ");
											strcat(parcel,sizes);
											strcat(parcel,"\n");
											write(1,parcel,strlen(parcel));
										}
									//	counting++;
									}
									else
									{
										char tty[]="ERROR IN NO OF ARGS ONLY 1\n";
										write(2,tty,strlen(tty));
									}
									_exit(0);

								}
								else
								{
									if(argval==NULL)
									{
										int hjk=execvp(ref,arg);
										if(hjk==-1)
										{
											char gg[]="WRONG COMMAND\n";
											write(2,gg,strlen(gg));
											_exit(0);
											break;
										}
									}
									else
									{
										int hjk=execvp(ref,arg);
										if(hjk==-1)
										{
											char gg[]="WRONG COMMAND\n";
											write(2,gg,strlen(gg));
											_exit(0);
											break;
										}
									}
								}

								close(fdin[1]);
								close(fdin[0]);
								close(fdout[1]);
								close(fdout[0]);
							}	
							else
							{
								close(fdpipe[2*(yop-1)+3]);
								if(forking==0)
								{
									int st;
									waitpid(pid,&st,WUNTRACED);
									if(WIFEXITED(st))
									{
										hist[histlen-1].status=-1;
									}
									else
									{
									}
								}
							//	if(forking==1 && redirectout==1)
							//	{
							//		int st;
							//		waitpid(pid,&st,WUNTRACED);
							//		if(WIFEXITED(st))
							//		{
							//			hist[histlen-1].status=-1;
							//		}
							//		else
							//		{
							//		}


							//	}
							}

						}
					}
						oddeven++;
				}
			}
		}
		if(sigchi==1)
		{
			write(1,fgh,strlen(fgh));
			sigchi=0;
		}
	}
	return 0;
}
