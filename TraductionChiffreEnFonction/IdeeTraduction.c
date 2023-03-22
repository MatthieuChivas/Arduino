#include <stdio.h>
#include <stdlib.h>

int main(){
    int numberCommand=520;
    
    int unite = numberCommand%10;
    int dizaine=numberCommand/10;
    dizaine=dizaine%10;
    int centaine=numberCommand/100;
    centaine=centaine%10;
    
    printf("number commande = %d \n",numberCommand);
    
    printf("centaine = %d \n",centaine);
    printf("dizaine = %d \n",dizaine);
    printf("unite = %d \n\n",unite);
    
    if(traductionBoutton(numberCommand)==5){
        printf("Enregistrer\n");
    }
    if(traductiontete(numberCommand)==2){
        printf("Tete haute\n");
    }
    if(traductionCorps(numberCommand)==0){
        printf("On avance a droite\n");
    }
    

    return 0;
}

int traductionBoutton(numberCommand){
    int centaine=numberCommand/100;
    centaine=centaine%10;
    return(centaine);
}

int traductiontete(numberCommand){
    int dizaine=numberCommand/10;
    dizaine=dizaine%10;
    return(dizaine);
}

int traductionCorps(numberCommand){
    int unite = numberCommand%10;
    return(unite);
}