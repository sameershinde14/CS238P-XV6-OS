/*

EXTRA FEATURES SUPPORTED - 
  1. Implement lists of commands, separated by ";"
  2. Implement sub shells by implementing "(" and ")"
  3. Implement running commands in the background by supporting "&"
*/

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

// Simplifed xv6 shell.

#define MAXARGS 10

// All commands have at least a type. Have looked at the type, the code
// typically casts the *cmd to some specific cmd type.
struct cmd {
  int type;          //  ' ' (exec), | (pipe), '<' or '>' for redirection
};

struct execcmd {
  int type;              // ' '
  char *argv[MAXARGS];   // arguments to the command to be exec-ed
};

struct redircmd {
  int type;          // < or > 
  struct cmd *cmd;   // the command to be run (e.g., an execcmd)
  char *file;        // the input/output file
  int mode;          // the mode to open the file with
  int fd;            // the file descriptor number to use for the file
};

struct pipecmd {
  int type;          // |
  struct cmd *left;  // left side of pipe
  struct cmd *right; // right side of pipe
};

int fork1(void);  // Fork but exits on failure.
struct cmd *parsecmd(char*);
int processcmd(char *, int *);

// Execute cmd.  Never returns.
void
runcmd(struct cmd *cmd)
{
  int p[2], r;
  struct execcmd *ecmd;
  struct pipecmd *pcmd;
  struct redircmd *rcmd;

  if(cmd == 0)
    exit(0);
  
  switch(cmd->type){
  default:
    fprintf(stderr, "unknown runcmd\n");
    exit(-1);

  case ' ':
    ecmd = (struct execcmd*)cmd;
    if(ecmd->argv[0] == 0) exit(0);
    execvp(ecmd->argv[0], ecmd->argv);
    fprintf(stderr, "execv failed\n");
    break;

  case '>':
    rcmd = (struct redircmd*)cmd;
    close(rcmd->fd);
    open(rcmd->file, rcmd->mode, 0666);
    runcmd(rcmd->cmd);
    break;

  case '<':
    rcmd = (struct redircmd*)cmd;
    close(rcmd->fd);
    open(rcmd->file, rcmd->mode, 0666);
    runcmd(rcmd->cmd);
    break;

  case '|':
    
    pcmd = (struct pipecmd*)cmd;

    pipe(p);

    int pid = fork1();
    if(pid == -1) {
        printf("Couldn't fork the parent process...");
        exit(EXIT_FAILURE);
    }

    if(pid == 0) {
        close(1);
        dup(p[1]);
        close(p[0]);
        close(p[1]);
        runcmd(pcmd->left);
      }


      wait(NULL);
      int pid2 = fork1();
      if(pid2 == 0) {
        close(0);
        dup(p[0]);
        close(p[0]);
        close(p[1]);
        runcmd(pcmd->right);
    }


    close(p[0]);
    close(p[1]);
    wait(NULL);
    break;
  }    
  
  exit(0);
}

int
getcmd(char *buf, int nbuf)
{
  
  if (isatty(fileno(stdin)))
    fprintf(stdout, "238P$ ");
  memset(buf, 0, nbuf);
  fgets(buf, nbuf, stdin);
  if(buf[0] == 0) // EOF
    return -1;
  return 0;
}

int
main(void)
{
  static char buf[100];
  int fd, r;

  // Read and run input commands.
  while(getcmd(buf, sizeof(buf)) >= 0){
    if(buf[0] == 'c' && buf[1] == 'd' && buf[2] == ' '){
      // Clumsy but will have to do for now.
      // Chdir has no effect on the parent if run in the child.
      buf[strlen(buf)-1] = 0;  // chop \n
      if(chdir(buf+3) < 0)
        fprintf(stderr, "cannot cd %s\n", buf+3);
      continue;
    }

    processcmd(buf, &r);

  }

  exit(0);
}

int 
processcmd(char *buf, int *r) {

  int i = 0, j = 0, len = 0;
  while(buf[i] != '\0') { len++; i++; }
  i = 0;
  while(buf[i] != '\0' && buf[i] != '(') i++;
  j = i + 1;
  while(buf[j] != '\0' && buf[j] != ')') j++;
  if(buf[i] != '\0' && buf[j] != '\0') 
  {
      int len1 = i + 1;
      if(len1 > 1) {
        //printf("len1: %d, i: %d, j: %d \n", len1, i, j);
        char buf1[len1];
        int ptr = 0;
        while(ptr < len1 - 1) {buf1[ptr] = buf[ptr]; ptr++;}
        buf1[len1 - 1] = '\0';
        processcmd(buf1, r);
      }

      int len2 = j - i - 1 + 1;
      if(len2 > 1) {
        //printf("len2: %d, i: %d, j: %d \n", len2, i, j);
        char buf2[len2];
        int ptr = 0;
        while(ptr < len2 - 1) {buf2[ptr] = buf[i + 1 + ptr]; ptr++;}
        buf2[len2 - 1] = '\0';
        int newr;
        processcmd(buf2, &newr);
      }

      int len3 = len - j - 1 + 1;
      if(len3 > 1) {
        //printf("len3: %d, i: %d, j: %d \n", len3, i, j);
        char buf3[len3];
        int ptr = 0;
        while(ptr < len3 - 1) {buf3[ptr] = buf[j + 1 + ptr]; ptr++;}
        buf3[len3 - 1] = '\0';
        processcmd(buf3, r);
      }
  }

  else
  {
    char *cmdbuf = strtok(buf, ";");
    while(cmdbuf) {
      int len = 0;
      while(cmdbuf[len] != '\0') {len++;}
      int itr = 0;
      while(itr < len && cmdbuf[itr] != '&') itr++;

      if(itr < len) {
        char backgroundp[itr + 1];
        int ptr = 0;
        while(ptr < itr) {backgroundp[ptr] = cmdbuf[ptr]; ptr++;}
        backgroundp[itr] = '\0';

        if(fork1() == 0) 
        {
          runcmd(parsecmd(backgroundp));
        }
        else 
        {
          char mainp[len - itr];
          ptr = 0;
          while(ptr < len) {mainp[ptr] = cmdbuf[itr + 1 + ptr]; ptr++;}
          mainp[len - itr] = '\0';
          if(fork1() == 0) runcmd(parsecmd(mainp));
          wait(r);
          cmdbuf = strtok(NULL, ";");
        }

      } else {
          if(fork1() == 0) runcmd(parsecmd(cmdbuf));
          wait(r);
          cmdbuf = strtok(NULL, ";");
      }
    }

  }

    return 1;
}

int
fork1(void)
{
  int pid;
  
  pid = fork();
  if(pid == -1)
    perror("fork");
  return pid;
}

struct cmd*
execcmd(void)
{
  struct execcmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = ' ';
  return (struct cmd*)cmd;
}

struct cmd*
redircmd(struct cmd *subcmd, char *file, int type)
{
  struct redircmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = type;
  cmd->cmd = subcmd;
  cmd->file = file;
  cmd->mode = (type == '<') ?  O_RDONLY : O_WRONLY|O_CREAT|O_TRUNC;
  cmd->fd = (type == '<') ? 0 : 1;
  return (struct cmd*)cmd;
}

struct cmd*
pipecmd(struct cmd *left, struct cmd *right)
{
  struct pipecmd *cmd;

  cmd = malloc(sizeof(*cmd));
  memset(cmd, 0, sizeof(*cmd));
  cmd->type = '|';
  cmd->left = left;
  cmd->right = right;
  return (struct cmd*)cmd;
}

// Parsing

char whitespace[] = " \t\r\n\v";
char symbols[] = "<|>";

int
gettoken(char **ps, char *es, char **q, char **eq)
{
  char *s;
  int ret;
  
  s = *ps;
  while(s < es && strchr(whitespace, *s))
    s++;
  if(q)
    *q = s;
  ret = *s;
  switch(*s){
  case 0:
    break;
  case '|':
  case '<':
    s++;
    break;
  case '>':
    s++;
    break;
  default:
    ret = 'a';
    while(s < es && !strchr(whitespace, *s) && !strchr(symbols, *s))
      s++;
    break;
  }
  if(eq)
    *eq = s;
  
  while(s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  return ret;
}

int
peek(char **ps, char *es, char *toks)
{
  char *s;
  
  s = *ps;
  while(s < es && strchr(whitespace, *s))
    s++;
  *ps = s;
  return *s && strchr(toks, *s);
}

struct cmd *parseline(char**, char*);
struct cmd *parsepipe(char**, char*);
struct cmd *parseexec(char**, char*);

// make a copy of the characters in the input buffer, starting from s through es.
// null-terminate the copy to make it a string.
char 
*mkcopy(char *s, char *es)
{
  int n = es - s;

  char *c = malloc(n+1);
  assert(c);
  strncpy(c, s, n);
  c[n] = 0;
  return c;
}

struct cmd*
parsecmd(char *s)
{
  char *es;
  struct cmd *cmd;

  es = s + strlen(s);
  cmd = parseline(&s, es);
  peek(&s, es, "");
  if(s != es){
    fprintf(stderr, "leftovers: %s\n", s);
    exit(-1);
  }
  return cmd;
}

struct cmd*
parseline(char **ps, char *es)
{
  struct cmd *cmd;
  cmd = parsepipe(ps, es);
  return cmd;
}

struct cmd*
parsepipe(char **ps, char *es)
{
  struct cmd *cmd;

  cmd = parseexec(ps, es);
  if(peek(ps, es, "|")){
    gettoken(ps, es, 0, 0);
    cmd = pipecmd(cmd, parsepipe(ps, es));
  }
  return cmd;
}

struct cmd*
parseredirs(struct cmd *cmd, char **ps, char *es)
{
  int tok;
  char *q, *eq;

  while(peek(ps, es, "<>")){
    tok = gettoken(ps, es, 0, 0);
    if(gettoken(ps, es, &q, &eq) != 'a') {
      fprintf(stderr, "missing file for redirection\n");
      exit(-1);
    }
    switch(tok){
    case '<':
      cmd = redircmd(cmd, mkcopy(q, eq), '<');
      break;
    case '>':
      cmd = redircmd(cmd, mkcopy(q, eq), '>');
      break;
    }
  }
  return cmd;
}

struct cmd*
parseexec(char **ps, char *es)
{
  char *q, *eq;
  int tok, argc;
  struct execcmd *cmd;
  struct cmd *ret;
  
  ret = execcmd();
  cmd = (struct execcmd*)ret;

  argc = 0;
  ret = parseredirs(ret, ps, es);
  while(!peek(ps, es, "|")){
    if((tok=gettoken(ps, es, &q, &eq)) == 0)
      break;
    if(tok != 'a') {
      fprintf(stderr, "syntax error\n");
      exit(-1);
    }
    cmd->argv[argc] = mkcopy(q, eq);
    argc++;
    if(argc >= MAXARGS) {
      fprintf(stderr, "too many args\n");
      exit(-1);
    }
    ret = parseredirs(ret, ps, es);
  }
  cmd->argv[argc] = 0;
  return ret;
}

