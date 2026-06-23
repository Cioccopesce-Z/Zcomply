#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_LINE 1024


int ip=0;
int deb=0;
int win=0;
int skip=1; //incrementa ip normalmente, ma se è 0, ip è già stato modificato da goto/if/while/for e non va incrementato ulteriormente
int var_count=0;
int var_count_c=0;
int program_size=0;
int r[5];

// funzioni utili

typedef struct {
	int line;
	char text[256];
} istruzione;

istruzione program[MAX_LINE];


//pre declared function
void execute(istruzione instr);
void interpretate();


typedef struct {
	char name[16];
	int value;
} variabile;

variabile var[256];

typedef struct {
    char name[16];
    char value[256];
} var_carattere;

var_carattere var_c[256];

int find_line_index(int line_number) {
    for (int i = 0; i < program_size; i++) {
        if (program[i].line == line_number)
            return i;
    }
    printf("Errore: riga %d non trovata\n", line_number);
    return program_size;  // fine programma
}




void print_c_reg(char *text,int i){
	if (i < 0 || i >= 5) {
    	printf("Registro r%d non valido\n", i);
    	return;
	}

	if(r[i]<1 || r[i]>26){
		printf("\n errore in expel_c impossibile stampare in linea: %s\n",text);
		return;
	}
	char letter = 'a' + (r[i]-1);
	printf("%c ",letter);
}

void print_c_var(char *text,int val){
	if(val < 1 || val > 26){
		printf("\n errore in expel_c impossibile stampare in linea: %s\n",text);
		return;
	}

	char letter = 'a' + (val-1);
	printf("%c ",letter);
}


char *get_var_c(char *name) {
    for (int i = 0; i < var_count_c; i++) {
        if (strcmp(var_c[i].name, name) == 0)
            return var_c[i].value;  // variabile trovata → ritorna il valore
    }
    return NULL;  // variabile NON trovata
}

int *get_var(char *name) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(var[i].name, name) == 0)
            return &var[i].value;  // variabile trovata → ritorna l'indirizzo
    }
    return NULL;  // variabile NON trovata
}

int starts_with(const char *str, const char *prefix) {
    return strncmp(str, prefix, strlen(prefix)) == 0;
}

// interazioni ext int


void exec_input(char *text) {
	char temp_var[16];
	int i=0;
	
	if(sscanf(text,"input_n r%d",&i) == 1){
			if(i < 0 || i >= 5){
    			printf("Registro r%d non valido\n", i);
    			return;
			}
		scanf("%d",&r[i]);
		return;
	}
	
	if(sscanf(text,"input_n %15s",temp_var) == 1){
		int *v = get_var(temp_var);
	    	if (!v) {
            printf("Variabile %s non dichiarata\n", temp_var);
            return;
        }
		scanf("%d",v);
		return;
	}
	
		printf("\n error on input_n line: %s \n",text);
}

void exec_expel_n(char *text) {
	char temp_var[16];
	int i=0;
	
	if(sscanf(text,"expel_n r%d",&i) == 1){
		printf("%d ",r[i]);
		return;
	}
	
	
	if(sscanf(text,"expel_n %15s",temp_var) == 1){
		int *v = get_var(temp_var);
	    	if (!v) {
            printf("Variabile %s non dichiarata\n", temp_var);
            return;
        }
		printf("%d ",*v);
		return;
	}
	
		printf("\n error on expel_n line: %s \n",text);
	
	
}

void exec_expel_c(char *text) {
	char temp_var[16];
    char str[32];
	int i=0;
	
    //contenuto registro come testo
	if(sscanf(text,"expel_c r%d",&i) == 1){
		print_c_reg(text, i);
		return;
	}
	
    //scritta direttamente
    if(sscanf(text,"expel_c \"%31[^\"]\"",str) == 1){

        if(strcmp(str,"cp") == 0){
    	printf("\n");
        return;
        }

        printf("%s",str);
        return;
    }
    

    if(sscanf(text,"expel_c %15s",temp_var) == 1){
        int *v = get_var(temp_var);

        if (!v) {
            char *vc = get_var_c(temp_var);
            printf("%s", vc ? vc : "");
            return;
        }

        print_c_var(text,*v);
        return;
    }
    
		printf("\n error on expel_c line: %s \n",text);
	
}

//operazioni coi registri

void exec_mov(char *text) {

    char src[16], dest[16];
    int num;

    if (sscanf(text, "mov %15s to %15s", src, dest) != 2) {
        printf("\nErrore sintassi MOV: %s\n", text);
        return;
    }

    int *src_ptr = NULL;
    int *dest_ptr = NULL;
    int src_val = 0;

    // ----------- SOURCE ------------

    // se è registro
    if (sscanf(src, "r%d", &num) == 1 && num >= 0 && num < 5) {
        src_val = r[num];
    }
    // se è numero
    else if (sscanf(src, "%d", &num) == 1) {
        src_val = num;
    }
    // se è variabile
    else {
        src_ptr = get_var(src);
        if (!src_ptr) {
            printf("Variabile %s non dichiarata\n", src);
            return;
        }
        src_val = *src_ptr;
    }

    // ----------- DEST ------------

    // se è registro
    if (sscanf(dest, "r%d", &num) == 1 && num >= 0 && num < 5) {
        r[num] = src_val;
        return;
    }

    // se è variabile
    dest_ptr = get_var(dest);
    if (!dest_ptr) {
        printf("Variabile %s non dichiarata\n", dest);
        return;
    }

    *dest_ptr = src_val;
}


void exec_swap(char *text) {
	char temp_var[16], varname[16];
	int i=0,o=0;
	
	//rX rX
	if(sscanf(text,"swap r%d r%d",&i,&o) == 2){
		int temp = r[o];
		r[o] = r[i];
		r[i] = temp;
		return;
	}

	//rX var
	if(sscanf(text,"swap r%d %15s",&i,temp_var) == 2){
		int *v = get_var(temp_var);
		if (!v) {
 		   printf("\nVariabile %s non dichiarata\n", temp_var);
    		return;
		}
		int temp = *v;
		*v = r[i];
		r[i] = temp;
		return;
	}
	
	//var rX
	if(sscanf(text,"swap %15s r%d",temp_var,&i) == 2){
		int *v = get_var(temp_var);
		if (!v) {
 		   printf("\nVariabile %s non dichiarata\n", temp_var);
    		return;
		}
		int temp = *v;
		*v = r[i];
		r[i] = temp;
		return;
	}
	
	//var var
	if(sscanf(text,"swap %15s %15s",temp_var,varname) == 2){
		int *v = get_var(temp_var);
		int *p = get_var(varname);
		if (!v) {
 		   printf("\nVariabile %s non dichiarata\n", temp_var);
    		return;
		}
		if (!p) {
 		   printf("\nVariabile %s non dichiarata\n", varname);
    		return;
		}
		int temp = *v;
		*v = *p;
		*p = temp;
		return;
	}
	

	
		printf("\n warning on SWAP line: %s\n",text);
}

void exec_decl(char *text){
	char temp_var[16];
	int i=0;

    //decl int var rX
	if(sscanf(text,"decl int %15s r%d",temp_var,&i) == 2){
		
		strcpy(var[var_count].name,temp_var);
		var[var_count].value = r[i];
		var_count++;
		return;
	}
	
	//decl int var N
	if(sscanf(text,"decl int %15s %d",temp_var,&i) == 2){
		
		strcpy(var[var_count].name,temp_var);
		var[var_count].value = i;
		var_count++;
		return;
	}

    if(sscanf(text,"decl char %15s \"%255[^\"]\"", temp_var, var_c[var_count_c].value) == 2){
        strcpy(var_c[var_count_c].name, temp_var);
        var_count_c++;
    return;
    }


		
	printf("\n error on DECL: %s\n",text);
	
}
	

//operazioni matematiche tra registri


void exec_add(char *text) {
    int rd, rs, imm;
    char varname[16], varname2[16];

    //rX rY
    if (sscanf(text, "add to r%d r%d", &rd, &rs) == 2) {
        r[rd] += r[rs];
        return;
    }

    //rX N
    if (sscanf(text, "add to r%d %d", &rd, &imm) == 2) {
        r[rd] += imm;
        return;
    }

    // rX var
	if (sscanf(text, "add to r%d %15s", &rd, varname) == 2) {

    	int temp;
    	if (sscanf(varname, "r%d", &temp) == 1) {
    	    return; // è un registro, questo caso non vale
    	}

    	int *v = get_var(varname);
    	if (!v) {
    	    printf("Variabile %s non dichiarata\n", varname);
    	    return;
    	}

    	r[rd] += *v;
    	return;
}

    
    //var rX
    if (sscanf(text, "add to %15s r%d", varname, &rd) == 2) {
        int *v = get_var(varname);
        if (!v) {
            printf("Variabile %s non dichiarata\n", varname);
            return;
        }

        *v += r[rd];
        return;
    }

    //var N
    if (sscanf(text, "add to %15s %d", varname, &rs) == 2) {
        int *v = get_var(varname);
        if (!v) {
            printf("Variabile %s non dichiarata\n", varname);
            return;
        }
        *v += rs;
        return;
    }
    
    //var var
    if (sscanf(text, "add to %15s %15s", varname2, varname) == 2) {
        int *v = get_var(varname);
        int *p = get_var(varname2);
        if (!v) {
            printf("Variabile %s non dichiarata\n", varname);
            return;
        }
        if (!p) {
            printf("Variabile %s non dichiarata\n", varname2);
            return;
        }

        *p += *v;
        return;
    }

    printf("\nErrore sintassi ADD: %s\n", text);
}

void exec_subb(char *text) {
    int rd, rs, imm;
    char varname[16], varname2[16];

    //rX rY
    if (sscanf(text, "subb from r%d r%d", &rd, &rs) == 2) {
        r[rd] -= r[rs];
        return;
    }

    //rX N
    if (sscanf(text, "subb from r%d %d", &rd, &imm) == 2) {
        r[rd] -= imm;
        return;
    }

    //rX var
    if (sscanf(text, "subb from r%d %15s", &rd, varname) == 2) {
        int *v = get_var(varname);
        if (!v) {
            printf("Variabile %s non dichiarata\n", varname);
            return;
        }
        r[rd] -= *v;
        return;
    }
    
    //var rX
    if (sscanf(text, "subb from %15s r%d", varname, &rd) == 2) {
        int *v = get_var(varname);
        if (!v) {
            printf("Variabile %s non dichiarata\n", varname);
            return;
        }

        *v -= r[rd];
        return;
    }
    
    //var N
    if (sscanf(text, "subb from %15s %d", varname, &rs) == 2) {
        int *v = get_var(varname);
        if (!v) {
            printf("Variabile %s non dichiarata\n", varname);
            return;
        }
        *v -= rs;
        return;
    }
    
    //var var
    if (sscanf(text, "subb from %15s %15s", varname2, varname) == 2) {
        int *v = get_var(varname);
        int *p = get_var(varname2);
        if (!v) {
            printf("Variabile %s non dichiarata\n", varname);
            return;
        }
        if (!p) {
            printf("Variabile %s non dichiarata\n", varname2);
            return;
        }

        *p -= *v;
        return;
    }

    printf("\n Errore sintassi SUBB: %s\n", text);
}


void exec_times(char *text) {
    int rd, rs, imm;
    char varname[16], varname2[16];

    // rX rY
    if (sscanf(text, "times r%d by r%d", &rd, &rs) == 2) {
        r[rd] *= r[rs];
        return;
    }

    // rX N
    if (sscanf(text, "times r%d by %d", &rd, &imm) == 2) {
        r[rd] *= imm;
        return;
    }

    // rX var
    if (sscanf(text, "times r%d by %15s", &rd, varname) == 2) {
        int *v = get_var(varname);
        if (!v) {
            printf("Variabile %s non dichiarata\n", varname);
            return;
        }
        r[rd] *= *v;
        return;
    }
    
    //var rX
    if (sscanf(text, "times %15s by r%d", varname, &rd) == 2) {
        int *v = get_var(varname);
        if (!v) {
            printf("Variabile %s non dichiarata\n", varname);
            return;
        }

        *v *= r[rd];
        return;
    }
    
    //  var N
    if (sscanf(text, "times %15s by %d", varname, &rs) == 2) {
        int *v = get_var(varname);
        if (!v) {
            printf("Variabile %s non dichiarata\n", varname);
            return;
        }
        *v *= rs;
        return;
    }
    
    //var var
    if (sscanf(text, "times %15s by %15s", varname2, varname) == 2) {
        int *v = get_var(varname);
        int *p = get_var(varname2);
        if (!v) {
            printf("Variabile %s non dichiarata\n", varname);
            return;
        }
        if (!p) {
            printf("Variabile %s non dichiarata\n", varname2);
            return;
        }

        *p *= *v;
        return;
    }

    printf("\nErrore sintassi TIMES: %s\n", text);
}


void exec_divide(char *text) {
    int rd, rs, imm;
    char varname[16], varname2[16];

    // rX rY
    if (sscanf(text, "divide r%d by r%d", &rd, &rs) == 2) {
        r[rd] /= r[rs];
        return;
    }

    // rX N
    if (sscanf(text, "divide r%d by %d", &rd, &imm) == 2) {
        r[rd] /= imm;
        return;
    }

    // rX var
    if (sscanf(text, "divide r%d by %15s", &rd, varname) == 2) {
        int *v = get_var(varname);
        if (!v) {
            printf("Variabile %s non dichiarata\n", varname);
            return;
        }
    if (*v == 0) {
    	printf("Errore: divisione per zero\n");
    	return;
	}

        r[rd] /= *v;
        return;
    }
    
    //var rX
    if (sscanf(text, "divide %15s by r%d", varname, &rd) == 2) {
        int *v = get_var(varname);
        if (!v) {
            printf("Variabile %s non dichiarata\n", varname);
            return;
        }
    if (*v == 0) {
    	printf("Errore: divisione per zero\n");
    	return;
	}

        *v /= r[rd];
        return;
    }
    
    // var N
    if (sscanf(text, "divide %15s by %d", varname, &rs) == 2) {
        int *v = get_var(varname);
        if (!v) {
            printf("Variabile %s non dichiarata\n", varname);
            return;
        }
        *v /= rs;
        return;
    }
    
    //var var
    if (sscanf(text, "divide %15s by %15s", varname2, varname) == 2) {
        int *v = get_var(varname);
        int *p = get_var(varname2);
        if (!v) {
            printf("Variabile %s non dichiarata\n", varname);
            return;
        }
        if (!p) {
            printf("Variabile %s non dichiarata\n", varname2);
            return;
        }
    if (*v == 0) {
    	printf("Errore: divisione per zero\n");
    	return;
	}

        *p /= *v;
        return;
    }

    printf("\nErrore sintassi DIVIDE: %s\n", text);
}

//condizioni logiche

void exec_if(char *text) {
    int rd, rs, ski=1;
    char varname[16], varname2[16], simbol[3];

    // rX N
    if (sscanf(text, "if(r%d %1[=<>] %d) %d", &rd, simbol, &rs, &ski) == 4) {
        if ((strcmp(simbol,"=")==0 && r[rd]!=rs) ||
            (strcmp(simbol,">")==0 && r[rd]<=rs) ||
            (strcmp(simbol,"<")==0 && r[rd]>=rs)) {
                //condizione falsa
            ip = ip + ski; // salto ski righe l'istruzione successiva
        }
        else{
        }
        return;
    }

    // rX rX
    if (sscanf(text, "if(r%d %1[=<>] r%d) %d", &rd, simbol, &rs, &ski) == 4) {
        if ((strcmp(simbol,"=")==0 && r[rd]!=r[rs]) ||
            (strcmp(simbol,">")==0 && r[rd]<=r[rs]) ||
            (strcmp(simbol,"<")==0 && r[rd]>=r[rs])) {
                //condizione falsa
            ip = ip + ski; // salto ski righe l'istruzione successiva
        }
        else{
        }
        return;
    }

    // rX var
    if (sscanf(text, "if(r%d %1[=<>] %15[^ )]) %d", &rd, simbol, varname, &ski) == 4) {
        int *v = get_var(varname);
        if (!v) { printf("Variabile %s non dichiarata\n", varname); return; }
        if ((strcmp(simbol,"=")==0 && r[rd]!=*v) ||
            (strcmp(simbol,">")==0 && r[rd]<=*v) ||
            (strcmp(simbol,"<")==0 && r[rd]>=*v)) {
                //condizione falsa
            ip = ip + ski; // salto ski righe l'istruzione successiva
        }
        else{
        }
        return;
    }

    // var N
    if (sscanf(text, "if(%15[^ )] %1[=<>] %d) %d", varname, simbol, &rd, &ski) == 4) {
        int *v = get_var(varname);
        if (!v) { printf("Variabile %s non dichiarata\n", varname); return; }
        if ((strcmp(simbol,"=")==0 && *v!=rd) ||
            (strcmp(simbol,">")==0 && *v<=rd) ||
            (strcmp(simbol,"<")==0 && *v>=rd)) {
                //condizione falsa
            ip = ip + ski; // salto ski righe l'istruzione successiva
        }
        else{
        }
        return;
    }


    // var rX
    if (sscanf(text, "if(%15[^ )] %1[=<>] r%d) %d", varname, simbol, &rd, &ski) == 4) {
        int *v = get_var(varname);
        if (!v) { printf("Variabile %s non dichiarata\n", varname); return; }
        if ((strcmp(simbol,"=")==0 && *v!=r[rd]) ||
            (strcmp(simbol,">")==0 && *v<=r[rd]) ||
            (strcmp(simbol,"<")==0 && *v>=r[rd])) {
                //condizione falsa
            ip = ip + ski; // salto ski righe l'istruzione successiva
        }
        else{
        }
        return;
    }

    // var var
    if (sscanf(text, "if(%15[^ )] %1[=<>] %15[^ )]) %d", varname, simbol, varname2, &ski) == 4) {
        int *v = get_var(varname);
        int *p = get_var(varname2);
        if (!v) { printf("Variabile %s non dichiarata\n", varname); return; }
        if (!p) { printf("Variabile %s non dichiarata\n", varname2); return; }
        if ((strcmp(simbol,"=")==0 && *v!=*p) ||
            (strcmp(simbol,">")==0 && *v<=*p) ||
            (strcmp(simbol,"<")==0 && *v>=*p)) {
                //condizione falsa
            ip = ip + ski; // salto ski righe l'istruzione successiva
        }
        else{
        }
        return;
    }
}


void exec_goto(char *text) {
    int line;
    sscanf(text, "goto %d", &line);
    ip = find_line_index(line);
	skip = 0; // non incrementare ip dopo questa istruzione
} 

void exec_while(char *text) {

    int rd, rs, body;
	char varname[16], varname2[16];
    char simbol[2];

	//rX rY
    if (sscanf(text, "while(r%d %1[=<>] r%d) %d", &rd, simbol, &rs, &body) == 4) {

        int while_ip = ip;

        while (
            (simbol[0] == '=' && r[rd] == r[rs]) ||
            (simbol[0] == '<' && r[rd] < r[rs]) ||
            (simbol[0] == '>' && r[rd] > r[rs])
        ) {
            ip = while_ip + 1;

            for (int i = 0; i < body; i++) {
                execute(program[ip]);
                ip++;
            }
        }

        ip = while_ip + body;
        skip = 0;
    }

	//rX var
	if(sscanf(text, "while(r%d %1[=<>] %15s) %d", &rd, simbol, varname2, &body) == 4) {
	
		int *p = get_var(varname2);
		if (!p) {
			printf("Variabile %s non dichiarata\n", varname2);
			return;
		}

		int while_ip = ip;

		while (
			(simbol[0] == '=' && r[rd] == *p) ||
			(simbol[0] == '<' && r[rd] < *p) ||
			(simbol[0] == '>' && r[rd] > *p)
		) {
			ip = while_ip + 1;

			for (int i = 0; i < body; i++) {
				execute(program[ip]);
				ip++;
			}
		}

		ip = while_ip + body;
		skip = 0;
	}

	//var rX
	if(sscanf(text, "while(%15s %1[=<>] r%d) %d", varname, simbol, &rd, &body) == 4) {
	
		int *p = get_var(varname);
		
		if (!p) {
			printf("Variabile %s non dichiarata\n", varname);
			return;
		}

		int while_ip = ip;

		while (
			(simbol[0] == '=' && *p == r[rd]) ||
			(simbol[0] == '<' && *p < r[rd]) ||
			(simbol[0] == '>' && *p > r[rd])
		) {
			ip = while_ip + 1;

			for (int i = 0; i < body; i++) {
				execute(program[ip]);
				ip++;
			}
		}

		ip = while_ip + body;
		skip = 0;
	}

	//var var
	if(sscanf(text, "while(%15s %1[=<>] %15s) %d", varname, simbol, varname2, &body) == 4) {
		int *v = get_var(varname);
		int *p = get_var(varname2);
		if (!v) {
			printf("Variabile %s non dichiarata\n", varname);
			return;
		}
		if (!p) {
			printf("Variabile %s non dichiarata\n", varname2);
			return;
		}

		int while_ip = ip;

		while (
			(simbol[0] == '=' && *v == *p) ||
			(simbol[0] == '<' && *v < *p) ||
			(simbol[0] == '>' && *v > *p)
		) {
			ip = while_ip + 1;

			for (int i = 0; i < body; i++) {
				execute(program[ip]);
				ip++;
			}
		}

		ip = while_ip + body;
		skip = 0;
	}
}

void exec_for(char *text) {

    int rd, rs, body;
	char varname[16], varname2[16];
    char simbol[2];

	//N
	if(sscanf(text, "for(%d %1[=<>] %d) %d", &rd, simbol, &rs, &body) == 4) {
	
		int for_ip = ip;

		while (
			(simbol[0] == '=' && rd == rs) ||
			(simbol[0] == '<' && rd < rs) ||
			(simbol[0] == '>' && rd > rs)
		) {
			ip = for_ip + 1;

			for (int i = 0; i < body; i++) {
				execute(program[ip]);
				ip++;
			}
		}

		ip = for_ip + body;
		skip = 0;
	}

	//rX rY
    if (sscanf(text, "for(r%d %1[=<>] r%d) %d", &rd, simbol, &rs, &body) == 4) {

        int for_ip = ip;

        while (
            (simbol[0] == '=' && r[rd] == r[rs]) ||
            (simbol[0] == '<' && r[rd] < r[rs]) ||
            (simbol[0] == '>' && r[rd] > r[rs])
        ) {
            ip = for_ip + 1;

            for (int i = 0; i < body; i++) {
                execute(program[ip]);
                ip++;
            }
        }

        ip = for_ip + body;
        skip = 0;
    }

	//rX var
	if(sscanf(text, "for(r%d %1[=<>] %15s) %d", &rd, simbol, varname2, &body) == 4) {
	
		int *p = get_var(varname2);
		if (!p) {
			printf("Variabile %s non dichiarata\n", varname2);
			return;
		}

		int for_ip = ip;

		while (
			(simbol[0] == '=' && r[rd] == *p) ||
			(simbol[0] == '<' && r[rd] < *p) ||
			(simbol[0] == '>' && r[rd] > *p)
		) {
			ip = for_ip + 1;

			for (int i = 0; i < body; i++) {
				execute(program[ip]);
				ip++;
			}
		}

		ip = for_ip + body;
		skip = 0;
	}

	//var rX
	if(sscanf(text, "for(%15s %1[=<>] r%d) %d", varname, simbol, &rd, &body) == 4) {
	
		int *p = get_var(varname);

		if (!p) {
			printf("Variabile %s non dichiarata\n", varname);
			return;
		}

		int for_ip = ip;

		while (
			(simbol[0] == '=' && *p == r[rd]) ||
			(simbol[0] == '<' && *p < r[rd]) ||
			(simbol[0] == '>' && *p > r[rd])
		) {
			ip = for_ip + 1;

			for (int i = 0; i < body; i++) {
				execute(program[ip]);
				ip++;
			}
		}

		ip = for_ip + body;
		skip = 0;
	}

    //var N to N
    if(sscanf(text, "for(%15s %d %1[=<>] %d) %d", varname, &rs, simbol, &rd, &body) == 5) {
        int *p = get_var(varname);
        if (!p) {
            printf("Variabile %s non dichiarata\n", varname);
            return;
        }

        int for_ip = ip;
        *p = rs;
        while (
			(simbol[0] == '=' && *p == rd) ||
			(simbol[0] == '<' && *p < rd) ||
			(simbol[0] == '>' && *p > rd)
		) {
			ip = for_ip + 1;

			for (int i = 0; i < body; i++) {
				execute(program[ip]);
                (*p)++;
				ip++;
			}
		}

        ip = for_ip + body;
        skip = 0;
    }

	//var var
	if(sscanf(text, "for(%15s %1[=<>] %15s) %d", varname, simbol, varname2, &body) == 4) {
		int *v = get_var(varname);
		int *p = get_var(varname2);
		if (!v) {
			printf("Variabile %s non dichiarata\n", varname);
			return;
		}
		if (!p) {
			printf("Variabile %s non dichiarata\n", varname2);
			return;
		}

		int for_ip = ip;

		while (
			(simbol[0] == '=' && *v == *p) ||
			(simbol[0] == '<' && *v < *p) ||
			(simbol[0] == '>' && *v > *p)
		) {
			ip = for_ip + 1;

			for (int i = 0; i < body; i++) {
				execute(program[ip]);
				ip++;
			}
		}

		ip = for_ip + body;
		skip = 0;
	}

}

void exec_delay(char *text) {
    int time;

    
    if (sscanf(text, "delay %dms", &time) == 1) {
    #ifdef _WIN32
        Sleep(time);  
    #else
        usleep(time * 1000);  // microsecondi
    #endif
        return;
    }

    if (sscanf(text, "delay %ds", &time) == 1) {
        int ms = time * 1000;
    #ifdef _WIN32
        Sleep(ms);  
    #else
        usleep(ms * 1000);
    #endif
        return;
    }

    printf("\nErrore sintassi DELAY: %s\n", text);
}

//funzioni

void exec_function(char *text) {

}

//sistema

void reset_program() {
    program_size = 0;
    ip = 0;
}


void exec_loadscript(char *text) {
	reset_program();
    char filename[128];

    if (sscanf(text, "loadscript %127s", filename) != 1) {
        printf("Errore sintassi loadscript\n");
        return;
    }

    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Impossibile aprire il file: %s\n", filename);
        return;
    }

    program_size = 0;
    ip = 0;

    char line[256];

    while (fgets(line, sizeof(line), file)) {

		//rimuove il newline finale
        line[strcspn(line, "\n")] = 0;

        if (strlen(line) == 0)
            continue;

        sscanf(line, "%d %[^\n]", 
               &program[program_size].line, 
               program[program_size].text);

        program_size++;
    }

    fclose(file);

    if(deb) printf("Script %s caricato. %d linee.\n", filename, program_size);

    interpretate();   // esegue subito lo script
}


void exec_comment(char *text) {
	//questa banalmente non fa nulla
}

void exec_kill(){
	exit(0);
}

void exec_debug(char *text) {
	char i[4];
	if(	sscanf(text,"debug %s",i)==1 ){
		if(strcmp(i,"on")==0){
			deb=1;
			return;
		}
		if(strcmp(i,"off")==0){
			deb=0;
			return;
		}
	}
	printf("\n error on line DEBUG: %s\n",text);
	return;
}

void exec_unix(char *text) {
	char i[4];
	if(	sscanf(text,"unix %s",i)==1 ){
		if(strcmp(i,"yes")==0){
			win=0;
			return;
		}
		if(strcmp(i,"no")==0){
			win=1;
			return;
		}
	}
	printf("\n error on line UNIX: %s\n",text);
	return;
}

void exec_cls(char *text) {
    if(win) system("cls");
    else system("clear");
}


//parser

void execute(istruzione instr) {
	
    if (starts_with(instr.text, "input_n")) exec_input(instr.text);
    else if (starts_with(instr.text, "expel_c")) exec_expel_c(instr.text);
    else if (starts_with(instr.text, "expel_n")) exec_expel_n(instr.text);
    
    else if (starts_with(instr.text, "mov")) exec_mov(instr.text);
    else if (starts_with(instr.text, "swap")) exec_swap(instr.text);
	else if (starts_with(instr.text, "decl")) exec_decl(instr.text);
    
    else if (starts_with(instr.text, "add to")) exec_add(instr.text);
    else if (starts_with(instr.text, "subb from")) exec_subb(instr.text);
    else if (starts_with(instr.text, "times")) exec_times(instr.text);
    else if (starts_with(instr.text, "divide")) exec_divide(instr.text);
    
    else if (starts_with(instr.text, "if")) exec_if(instr.text);
    else if (starts_with(instr.text, "goto")) exec_goto(instr.text);
    else if (starts_with(instr.text, "while")) exec_while(instr.text);
	else if (starts_with(instr.text, "for")) exec_for(instr.text);
    
    else if (starts_with(instr.text, "function")) exec_function(instr.text);
	else if (starts_with(instr.text, "loadscript")) exec_loadscript(instr.text);
    else if (starts_with(instr.text, "delay")) exec_delay(instr.text);
	else if (starts_with(instr.text, "cls")) exec_cls(instr.text);

	else if (starts_with(instr.text, ">//")) exec_comment(instr.text);
	else if (starts_with(instr.text, "debug")) exec_debug(instr.text);
	else if (starts_with(instr.text, "unix")) exec_unix(instr.text);
    else if (starts_with(instr.text, "kill")) exec_kill();
	
	else printf("\nComando non riconosciuto: %s\n", instr.text);
}

// leggi programma

void read_begin(){
	start: ;
	if(win) system("cls");
	ip=0;
	char line[256];
	int i = 0;
	for (int i = 0; i < 5; i++) r[i] = 0;

	while (1) {
		
		printf("[ ");
	    fgets(line, sizeof(line), stdin);

		// rimuove il newline finale
		line[strcspn(line, "\n")] = 0;

   		if (strcmp(line, "run") == 0) {
			for(int i=0;i<20;i++){
				printf(".");
			}

				printf("\n");

			for(int i=0;i<9;i++){
				printf(".");
			}
			printf("\n");
			break;
		}
		if (strcmp(line, "cls") == 0) {
		    goto start;
		}

		sscanf(line, "%d %[^\n]", &program[i].line, program[i].text);
		i++;
	}
	program_size = i;
}

// esegui il programma

void interpretate() {
	while (ip < program_size) {
			if(deb) printf("\n line executed: %d \n",ip);
            if(deb)printf("IP: %d\n", ip);
    		execute(program[ip]);
    		if(skip) ip++; 
            //if skip è 1, incrementa ip normalmente. Se è 0, ip è già stato modificato da goto/if/while/for e non va incrementato ulteriormente
			skip=1;
		}
}


int main(int argc, char *argv[]) {
    #ifdef _WIN32
        system("cls"); 
    #else
        system("clear");
    #endif

    if(argc > 1) {
        char buffer[1024];
        sprintf(buffer, "loadscript %s", argv[1]);
        exec_loadscript(buffer);
        return 0;
    }

    read_begin();
    interpretate();

    return 0;
}