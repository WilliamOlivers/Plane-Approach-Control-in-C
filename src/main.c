#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <string.h>
#include <time.h>
#include <SDL_mixer.h>
#include <windows.h>
#include <stdbool.h>

typedef enum etatInterface
{
    MENU,
    AFF,
    IDENT,
    EXIT,
    UTIL,
    CARTE,
    END
} etatInterface;

typedef struct Images
{
    SDL_Texture *menuIMG;
    SDL_Texture *identificationIMG;
    SDL_Texture *afficherIMG;
    SDL_Texture *utilisateurIMG;
    SDL_Texture *carteIMG;
    SDL_Texture *quitterIMG;
} Images;

typedef struct Button
{
    int min_x, max_x, min_y, max_y;
} Button;

typedef struct Buttons
{
    // Boutons Navigation
    Button afficherBtn;
    Button identificationBtn;
    Button quitterBtn;
    Button validerBtn;
    Button retourBtn;
    Button carteBtn;
    // Boutons Secondaires
    Button immatriculation;
    Button vitesse;
    Button cap;
    Button fuel;
} Buttons;

typedef struct rectTexture
{
    SDL_Texture *immatriculationTexte;
    SDL_Texture *vitesseTexte;
    SDL_Texture *capTexte;
    SDL_Texture *fuelTexte;

    SDL_Texture *liste1Texte;
    SDL_Texture *liste2Texte;
    SDL_Texture *liste3Texte;
    SDL_Texture *liste4Texte;
    SDL_Texture *heureRectTexte;
    SDL_Texture *nbRectTexte;
    SDL_Texture *immatriculationUtilisateurTexte;
    SDL_Texture *pisteUtilisateurTexte;

    SDL_Texture *errorFieldTexte;

    SDL_Texture *lightTexte;

} rectTexture;

typedef struct Surface
{
    SDL_Surface *immatriculationSurfaceTexte;
    SDL_Surface *vitesseSurfaceTexte;
    SDL_Surface *capSurfaceTexte;
    SDL_Surface *fuelSurfaceTexte;

    SDL_Surface *liste1SurfaceTexte;
    SDL_Surface *liste2SurfaceTexte;
    SDL_Surface *liste3SurfaceTexte;
    SDL_Surface *liste4SurfaceTexte;

    SDL_Surface *heureRectSurfaceTexte;
    SDL_Surface *nbRectSurfaceTexte;
    SDL_Surface *immatriculationUtilisateurSurfaceTexte;
    SDL_Surface *pisteUtilisateurSurfaceTexte;

    SDL_Surface *errorFieldSurfaceTexte;

    SDL_Surface *lightSurface;
} Surface;

#define MAX_IMMA_TAILLE 5
#define MAX_VAL_TAILLE 3

// Fonctions permettant de cast string  -> int
int parseInt(char *chars);
int powInt(int x, int y);

void FreeImages(Images *images);
void DestroyTexture(rectTexture *texture);
void FreeSurface(Surface *surface);
void chargerImages(Images *images, SDL_Renderer *renderer);
void initButtons(Buttons *btns);
void creerEtAfficherUnRectangle(int x, int y, int h, int w, SDL_Renderer *renderer, SDL_Surface *surface, int r, int g, int b, int t);
int afficherImage(SDL_Renderer *renderer, SDL_Texture *texture);
short int mouseABouton(int x, int y, Buttons btns);

//----------------  | Programme interne | ----------------
typedef struct avion
{
    signed char immatriculation[256];
    int vitesse;
    int cap;
    int fuel;
    int piste;
    char date[256];
    char liste[1000];
} avion;
#define num 20
avion p[num] = {"", 0, 0, 0, 0, "", ""};
int nb = 0;
char heureDirect[50];
//--------------------------------------------------------------------------- clear_files
void clear_files();
//--------------------------------------------------------------------------- Fonction clear
void clear();
//--------------------------------------------------------------------------- Fonction afficher
void afficher();
//--------------------------------------------------------------------------- Fonction menu
void menu();
//--------------------------------------------------------------------------- Fonction creation_avion
void creation_avion();
//--------------------------------------------------------------------------- Fonction stock
void stock();
//--------------------------------------------------------------------------- Fonction prise_en_charge
void prise_en_charge();

// Fin programme interne
int main(int argc, char *argv[])
{
    // coordonées curseur
    int x, y = 0;

    clear_files(); // Effacer les fichiers Excel précédants

    // Déclaration des structures
    Images images;
    rectTexture texture;
    Surface surface;
    Buttons btns;
    //

    etatInterface eI = MENU; // Etat initial ==> Menu

    unsigned int quitter = 0,
                 immatriculationTextLength = 0,
                 vitesseTextLength = 0,
                 capTextLength = 0,
                 fuelTextLength = 0;

    char *immatriculationTextInput = (char *)malloc(50),
         *vitesseTextInput = (char *)malloc(50),
         *capTextInput = (char *)malloc(50),
         *fuelTextInput = (char *)malloc(50);
    bool isImmatriculationTexteInput = FALSE,
         isVitesseTexteInput = FALSE,
         isCapTexteInput = FALSE,
         isFuelTexteInput = FALSE,
         errorFieldBool = FALSE,
         errorCapBool = FALSE,
         errorFuelBool = FALSE;

    typedef struct Lights
    {
        int xLeftGreen;
        int xRightGreen;
        int xLeftRed;
        int xRightRed;
        int yLeftGreen;
        int yRightGreen;
        int yLeftRed;
        int yRightRed;
    } Lights;

    Lights lights;

    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    // PARTIE AUDIO
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) == -1) // Initialisation de l'API Mixer
    {
        printf("%s", Mix_GetError());
    }
    Mix_Music *musique;
    Mix_Music *buttonSound;
    Mix_Music *validateSound;
    Mix_Music *errorSound;
    Mix_Music *errorSound2;
    Mix_Music *endSound;
    Mix_Music *alertSound;

    musique = Mix_LoadMUS("sound/theParade.mp3");
    buttonSound = Mix_LoadMUS("sound/buttonSound.mp3");
    validateSound = Mix_LoadMUS("sound/validateSound.mp3");
    errorSound = Mix_LoadMUS("sound/errorSound.mp3");
    errorSound2 = Mix_LoadMUS("sound/errorSound2.mp3");
    endSound = Mix_LoadMUS("sound/endSound.mp3");
    alertSound = Mix_LoadMUS("sound/alertSound.mp3");

    Mix_VolumeMusic(MIX_MAX_VOLUME);
    //      FIN PARTIE AUDIO
    initButtons(&btns);

    SDL_Window *ecran = SDL_CreateWindow("AirControl",
                                         SDL_WINDOWPOS_UNDEFINED,
                                         SDL_RENDERER_ACCELERATED,
                                         800, 600, SDL_WINDOW_RESIZABLE);
    SDL_SetWindowResizable(ecran, SDL_FALSE);
    if (ecran == NULL)
    {
        printf("Error : %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(ecran, -1, SDL_RENDERER_ACCELERATED);

    if (renderer == NULL)
    {
        printf("Error : %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    // Activation de l'alpha (transparence)
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    //
    SDL_Surface *cursorIMG = IMG_Load("images/cursor.png");
    SDL_Texture *cursorTexte = SDL_CreateTextureFromSurface(renderer, cursorIMG);
    SDL_Surface *planeIMG = IMG_Load("images/plane1.png");
    SDL_Texture *planeTexte = SDL_CreateTextureFromSurface(renderer, planeIMG);
    SDL_FreeSurface(planeIMG);
    SDL_ShowCursor(SDL_DISABLE);
    //  Chargement des images
    chargerImages(&images, renderer);

    // Définir le rectangle de la
    SDL_Rect immatriculation, vitesse, cap, fuel, liste1, liste2, liste3, liste4, heureRect, nbRect, terminer, quitterUtilisateur, immatriculationView, vitesseView, capView, fuelView, pisteUtilisateur, immatriculationUtilisateur, errorField, plane1;

    immatriculation.x = 255;
    vitesse.x = 190;
    cap.x = 170;
    fuel.x = 140;
    immatriculation.y = 152;
    vitesse.y = 235;
    cap.y = 320;
    fuel.y = 403;
    immatriculation.w = vitesse.w = cap.w = fuel.w = 10;
    immatriculation.h = vitesse.h = cap.h = fuel.h = 25;
    //
    immatriculationView.x = 245;
    vitesseView.x = 180;
    capView.x = 160;
    fuelView.x = 130;
    immatriculationView.y = 152;
    vitesseView.y = 235;
    capView.y = 320;
    fuelView.y = 403;
    immatriculationView.w = 225;
    vitesseView.w = 290;
    capView.w = 310;
    fuelView.w = 340;
    immatriculationView.h = vitesseView.h = capView.h = fuelView.h = 25;
    // Affichage
    liste1.x = 200;
    liste1.y = 190;
    liste1.w = 400;
    liste1.h = 20;

    liste2.x = 200;
    liste2.y = 290;
    liste2.w = 400;
    liste2.h = 20;

    liste3.x = 200;
    liste3.y = 390;
    liste3.w = 400;
    liste3.h = 20;

    liste4.x = 200;
    liste4.y = 490;
    liste4.w = 400;
    liste4.h = 20;

    // Heure et nombre aéronefs identifiées
    heureRect.x = 600;
    heureRect.y = 40;
    heureRect.w = 80;
    heureRect.h = 20;

    nbRect.x = 754;
    nbRect.y = 85;
    nbRect.w = 15;
    nbRect.h = 20;
    // Erreurs

    errorField.x = 100;
    errorField.y = 500;
    errorField.w = 200;
    errorField.h = 20;

    // Utilisateur
    immatriculationUtilisateur.x = 209;
    immatriculationUtilisateur.y = 184;
    immatriculationUtilisateur.w = 70;
    immatriculationUtilisateur.h = 20;

    pisteUtilisateur.x = 196;
    pisteUtilisateur.y = 257;
    pisteUtilisateur.w = 15;
    pisteUtilisateur.h = 20;

    quitterUtilisateur.x = 616;
    quitterUtilisateur.y = 13;
    quitterUtilisateur.w = 160;
    quitterUtilisateur.h = 50;

    //
    terminer.x = 420;
    terminer.y = 542;
    terminer.w = 160;
    terminer.h = 50;
    //
    plane1.x = 0;
    plane1.y = 0;
    plane1.w = 20;
    plane1.h = 20;
    //
    TTF_Font *font = TTF_OpenFont("font/ARIAL.TTF", 30);
    if (font == NULL)
    {
        printf("Error : %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_Color black = {0, 0, 0};
    SDL_Color white = {255, 255, 255};

    Surface surf;
    rectTexture affichage;

    while (eI != EXIT)
    {
        static int end = 0;
        if (end == 2)
        {
            eI = EXIT;
        }
        time_t s, val = 1;
        struct tm *current_time;
        s = time(NULL);
        current_time = localtime(&s);
        sprintf(heureDirect, "%02d:%02d:%02d",
                current_time->tm_hour,
                current_time->tm_min,
                current_time->tm_sec);
        int letterWidth = 15;
        switch (eI)
        {
        case MENU:
            if (afficherImage(renderer, images.menuIMG) == EXIT_FAILURE)
                return EXIT_FAILURE;
            break;
        case AFF:
            if (afficherImage(renderer, images.afficherIMG) == EXIT_FAILURE)
                return EXIT_FAILURE;

            SDL_RenderCopy(renderer, affichage.liste1Texte, NULL, &liste1);
            SDL_RenderCopy(renderer, affichage.liste2Texte, NULL, &liste2);
            SDL_RenderCopy(renderer, affichage.liste3Texte, NULL, &liste3);
            SDL_RenderCopy(renderer, affichage.liste4Texte, NULL, &liste4);

            SDL_RenderDrawRect(renderer, &liste1);
            SDL_RenderDrawRect(renderer, &liste2);
            SDL_RenderDrawRect(renderer, &liste3);
            SDL_RenderDrawRect(renderer, &liste4);
            //
            if (affichage.liste1Texte != NULL)
                SDL_DestroyTexture(affichage.liste1Texte);
            if (affichage.liste2Texte != NULL)
                SDL_DestroyTexture(affichage.liste2Texte);
            if (affichage.liste3Texte != NULL)
                SDL_DestroyTexture(affichage.liste3Texte);
            if (affichage.liste4Texte != NULL)
                SDL_DestroyTexture(affichage.liste4Texte);
            surf.liste1SurfaceTexte = TTF_RenderText_Blended(font, p[nb - 1].liste, black);

            affichage.liste1Texte = SDL_CreateTextureFromSurface(renderer, surf.liste1SurfaceTexte);
            if (nb >= 2)
            {
                surf.liste2SurfaceTexte = TTF_RenderText_Blended(font, p[nb - 2].liste, black);
                affichage.liste2Texte = SDL_CreateTextureFromSurface(renderer, surf.liste2SurfaceTexte);
            }
            if (nb >= 3)
            {
                surf.liste3SurfaceTexte = TTF_RenderText_Blended(font, p[nb - 3].liste, black);
                affichage.liste3Texte = SDL_CreateTextureFromSurface(renderer, surf.liste3SurfaceTexte);
            }
            if (nb >= 4)
            {
                surf.liste4SurfaceTexte = TTF_RenderText_Blended(font, p[nb - 4].liste, black);
                affichage.liste4Texte = SDL_CreateTextureFromSurface(renderer, surf.liste4SurfaceTexte);
            }

            break;
        case IDENT:
            if (afficherImage(renderer, images.identificationIMG) == EXIT_FAILURE)
                return EXIT_FAILURE;
            SDL_RenderCopy(renderer, affichage.immatriculationTexte, NULL, &immatriculation);
            SDL_RenderCopy(renderer, affichage.vitesseTexte, NULL, &vitesse);
            SDL_RenderCopy(renderer, affichage.capTexte, NULL, &cap);
            SDL_RenderCopy(renderer, affichage.fuelTexte, NULL, &fuel);
            SDL_RenderCopy(renderer, affichage.heureRectTexte, NULL, &heureRect);
            SDL_RenderCopy(renderer, affichage.nbRectTexte, NULL, &nbRect);

            if (affichage.heureRectTexte != NULL)
                SDL_DestroyTexture(affichage.heureRectTexte);
            if (affichage.nbRectTexte != NULL)
                SDL_DestroyTexture(affichage.nbRectTexte);

            surf.heureRectSurfaceTexte = TTF_RenderText_Blended(font, heureDirect, black);
            char str[80];
            sprintf(str, "%d", nb);
            surf.nbRectSurfaceTexte = TTF_RenderText_Blended(font, str, black);
            affichage.heureRectTexte = SDL_CreateTextureFromSurface(renderer, surf.heureRectSurfaceTexte);
            affichage.nbRectTexte = SDL_CreateTextureFromSurface(renderer, surf.nbRectSurfaceTexte);

            break;
        case UTIL:
            if (afficherImage(renderer, images.utilisateurIMG) == EXIT_FAILURE)
                return EXIT_FAILURE;
            SDL_RenderDrawRect(renderer, &quitterUtilisateur);
            SDL_RenderCopy(renderer, affichage.immatriculationUtilisateurTexte, NULL, &immatriculationUtilisateur);
            SDL_RenderCopy(renderer, affichage.pisteUtilisateurTexte, NULL, &pisteUtilisateur);
            if (affichage.immatriculationUtilisateurTexte != NULL)
                SDL_DestroyTexture(affichage.immatriculationUtilisateurTexte);
            if (affichage.pisteUtilisateurTexte != NULL)
                SDL_DestroyTexture(affichage.pisteUtilisateurTexte);
            surf.immatriculationUtilisateurSurfaceTexte = TTF_RenderText_Blended(font, immatriculationTextInput, black);
            affichage.immatriculationUtilisateurTexte = SDL_CreateTextureFromSurface(renderer, surf.immatriculationSurfaceTexte);
            char pisteVal[10];
            sprintf(pisteVal, "%d", p[nb - 1].piste);
            surf.pisteUtilisateurSurfaceTexte = TTF_RenderText_Blended(font, pisteVal, black);
            affichage.pisteUtilisateurTexte = SDL_CreateTextureFromSurface(renderer, surf.pisteUtilisateurSurfaceTexte);
            break;

        case CARTE:
            if (afficherImage(renderer, images.carteIMG) == EXIT_FAILURE)
                return EXIT_FAILURE;
            if (p[nb - 1].piste == 3)
            {
                plane1.y -= 1;
                SDL_Rect dstrect = {plane1.x - 22, plane1.y, 70, 70};
                SDL_RenderCopy(renderer, planeTexte, NULL, &dstrect);
                if (plane1.y <= 100)
                {
                    plane1.y = 565;
                }

                SDL_Delay(20);
            }
            else
                creerEtAfficherUnRectangle(plane1.x, plane1.y, plane1.h, plane1.w, renderer, surf.lightSurface, 134, 131, 131, 255);
            //  Lumières debut de piste (verte)
            creerEtAfficherUnRectangle(lights.xLeftGreen, lights.yLeftGreen, 5, 5, renderer, surf.lightSurface, 0, 255, 0, 255);
            creerEtAfficherUnRectangle(lights.xRightGreen, lights.yRightGreen, 5, 5, renderer, surf.lightSurface, 0, 255, 0, 255);
            // Lumières fin de piste (rouge)
            creerEtAfficherUnRectangle(lights.xLeftRed, lights.yLeftRed, 5, 5, renderer, surf.lightSurface, 255, 0, 0, 255);
            creerEtAfficherUnRectangle(lights.xRightRed, lights.yRightRed, 5, 5, renderer, surf.lightSurface, 255, 0, 0, 255);

            break;
        case END:
            if (afficherImage(renderer, images.quitterIMG) == EXIT_FAILURE)
                return EXIT_FAILURE;
            if (end == 1)
            {
                Mix_PlayMusic(endSound, 1);
                SDL_Delay(2000);
            }
            end++;
            break;

        default:
            break;
        }

        SDL_Event event;
        SDL_PollEvent(&event);
        switch (event.type)
        {
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_BACKSPACE)
            {
                if (eI == IDENT && isImmatriculationTexteInput && immatriculationTextLength > 0)
                {
                    immatriculationTextLength--;
                    immatriculation.w -= letterWidth;
                    immatriculationTextInput[strlen(immatriculationTextInput) - 1] = 0;
                    surf.immatriculationSurfaceTexte = TTF_RenderText_Blended(font, immatriculationTextInput, black);
                    affichage.immatriculationTexte = SDL_CreateTextureFromSurface(renderer, surf.immatriculationSurfaceTexte);
                }
                else if (eI == IDENT && isVitesseTexteInput && vitesseTextLength > 0)
                {
                    vitesseTextLength--;
                    vitesse.w -= letterWidth;
                    vitesseTextInput[strlen(vitesseTextInput) - 1] = 0;
                    surf.vitesseSurfaceTexte = TTF_RenderText_Blended(font, vitesseTextInput, black);
                    affichage.vitesseTexte = SDL_CreateTextureFromSurface(renderer, surf.vitesseSurfaceTexte);
                }
                else if (eI == IDENT && isCapTexteInput && capTextLength > 0)
                {
                    capTextLength--;
                    cap.w -= letterWidth;
                    capTextInput[strlen(capTextInput) - 1] = 0;
                    surf.capSurfaceTexte = TTF_RenderText_Blended(font, capTextInput, black);
                    affichage.capTexte = SDL_CreateTextureFromSurface(renderer, surf.capSurfaceTexte);
                }
                else if (eI == IDENT && isFuelTexteInput && fuelTextLength > 0)
                {
                    fuelTextLength--;
                    fuel.w -= letterWidth;
                    fuelTextInput[strlen(fuelTextInput) - 1] = 0;
                    surf.fuelSurfaceTexte = TTF_RenderText_Blended(font, fuelTextInput, black);
                    affichage.fuelTexte = SDL_CreateTextureFromSurface(renderer, surf.fuelSurfaceTexte);
                }
                if (parseInt(capTextInput) <= 360)
                {
                    errorCapBool = FALSE;
                }
                if (parseInt(fuelTextInput) <= 100)
                {
                    errorFuelBool = FALSE;
                }
            }
            else if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_RETURN)
            {
                if (isImmatriculationTexteInput == TRUE)
                {
                    isImmatriculationTexteInput = FALSE;
                    isVitesseTexteInput = TRUE;
                    isCapTexteInput = FALSE;
                    isFuelTexteInput = FALSE;
                }
                else if (isVitesseTexteInput == TRUE)
                {
                    isImmatriculationTexteInput = FALSE;
                    isVitesseTexteInput = FALSE;
                    isCapTexteInput = TRUE;
                    isFuelTexteInput = FALSE;
                }
                else if (isCapTexteInput == TRUE)
                {
                    isImmatriculationTexteInput = FALSE;
                    isVitesseTexteInput = FALSE;
                    isCapTexteInput = FALSE;
                    isFuelTexteInput = TRUE;
                }
                else if (isFuelTexteInput == TRUE && event.key.keysym.sym != SDLK_RETURN)
                {
                    isImmatriculationTexteInput = TRUE;
                    isVitesseTexteInput = FALSE;
                    isCapTexteInput = FALSE;
                    isFuelTexteInput = FALSE;
                }
            }
            else if (event.key.keysym.sym == SDLK_UP)
            {
                if (isImmatriculationTexteInput == TRUE)
                {
                    isImmatriculationTexteInput = FALSE;
                    isVitesseTexteInput = FALSE;
                    isCapTexteInput = FALSE;
                    isFuelTexteInput = TRUE;
                }
                else if (isVitesseTexteInput == TRUE)
                {
                    isImmatriculationTexteInput = TRUE;
                    isVitesseTexteInput = FALSE;
                    isCapTexteInput = FALSE;
                    isFuelTexteInput = FALSE;
                }
                else if (isCapTexteInput == TRUE)
                {
                    isImmatriculationTexteInput = FALSE;
                    isVitesseTexteInput = TRUE;
                    isCapTexteInput = FALSE;
                    isFuelTexteInput = FALSE;
                }
                else if (isFuelTexteInput == TRUE)
                {
                    isImmatriculationTexteInput = FALSE;
                    isVitesseTexteInput = FALSE;
                    isCapTexteInput = TRUE;
                    isFuelTexteInput = FALSE;
                }
            }
            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                eI = END;
            }
            break;
        case SDL_TEXTINPUT:

            if (eI == IDENT && isImmatriculationTexteInput && immatriculationTextLength < MAX_IMMA_TAILLE)
            {
                immatriculation.w += letterWidth;
                immatriculationTextLength++;
                if (immatriculationTextLength <= 1)
                {
                    strcpy(immatriculationTextInput, event.text.text);
                }
                else
                {
                    strcat(immatriculationTextInput, event.text.text);
                }
                surf.immatriculationSurfaceTexte = TTF_RenderText_Blended(font, immatriculationTextInput, black);
                affichage.immatriculationTexte = SDL_CreateTextureFromSurface(renderer, surf.immatriculationSurfaceTexte);
            }
            else if (eI == IDENT && isVitesseTexteInput && vitesseTextLength < MAX_VAL_TAILLE)
            {
                vitesse.w += letterWidth;
                vitesseTextLength++;
                if (vitesseTextLength <= 1)
                {
                    strcpy(vitesseTextInput, event.text.text);
                }
                else
                {
                    strcat(vitesseTextInput, event.text.text);
                }
                surf.vitesseSurfaceTexte = TTF_RenderText_Blended(font, vitesseTextInput, black);
                affichage.vitesseTexte = SDL_CreateTextureFromSurface(renderer, surf.vitesseSurfaceTexte);
            }
            else if (eI == IDENT && isCapTexteInput && capTextLength < MAX_VAL_TAILLE)
            {
                cap.w += letterWidth;
                capTextLength++;
                if (capTextLength <= 1)
                {
                    strcpy(capTextInput, event.text.text);
                }
                else
                {
                    strcat(capTextInput, event.text.text);
                }
                surf.capSurfaceTexte = TTF_RenderText_Blended(font, capTextInput, black);
                affichage.capTexte = SDL_CreateTextureFromSurface(renderer, surf.capSurfaceTexte);
            }
            else if (eI == IDENT && isFuelTexteInput && fuelTextLength < MAX_VAL_TAILLE)
            {
                fuel.w += letterWidth;
                fuelTextLength++;
                if (fuelTextLength <= 1)
                {
                    strcpy(fuelTextInput, event.text.text);
                }
                else
                {
                    strcat(fuelTextInput, event.text.text);
                }
                surf.fuelSurfaceTexte = TTF_RenderText_Blended(font, fuelTextInput, black);
                affichage.fuelTexte = SDL_CreateTextureFromSurface(renderer, surf.fuelSurfaceTexte);
            }
            if (eI == IDENT && immatriculationTextLength != 0 && vitesseTextLength != 0 && capTextLength != 0 && fuelTextLength != 0)
            {
                errorFieldBool = FALSE;
            }
            if (parseInt(capTextInput) <= 360)
            {
                errorCapBool = FALSE;
            }
            else if (parseInt(capTextInput) > 360)
            {
                errorCapBool = TRUE;
                Mix_PlayMusic(errorSound2, 1);
            }
            if (parseInt(fuelTextInput) <= 100)
            {
                errorFuelBool = FALSE;
            }
            else if (parseInt(fuelTextInput) > 100)
            {
                errorFuelBool = TRUE;
                Mix_PlayMusic(errorSound2, 1);
            }

            break;
        case SDL_MOUSEBUTTONDOWN:
            if (eI == AFF)
            {
                int x = event.button.x, y = event.button.y;

                // Cliquer sur la bouton 'Retour'
                if (x >= btns.retourBtn.min_x && x <= btns.retourBtn.max_x &&
                    y >= btns.retourBtn.min_y && y <= btns.retourBtn.max_y)
                {
                    Mix_PlayMusic(buttonSound, 1);
                    eI = MENU;
                }
            }
            if (eI == UTIL)
            {

                int x = event.button.x, y = event.button.y;
                if (x >= btns.carteBtn.min_x && x <= btns.carteBtn.max_x &&
                    y >= btns.carteBtn.min_y && y <= btns.carteBtn.max_y)
                {
                    Mix_PlayMusic(buttonSound, 1);
                    eI = CARTE;
                }
            }

            if (eI == CARTE)
            {
                int x = event.button.x, y = event.button.y;
                if (p[nb - 1].piste == 3)
                {
                    plane1.x = 380;
                    plane1.y = 565;
                    //// Lumières debut de piste (verte)
                    lights.xLeftGreen = 375;
                    lights.xRightGreen = 402;
                    lights.yLeftGreen = 480;
                    lights.yRightGreen = 480;
                    // Lumières fin de piste (rouge)
                    lights.xLeftRed = 375;
                    lights.xRightRed = 402;
                    lights.yLeftRed = 110;
                    lights.yRightRed = 110;
                }
                else if (p[nb - 1].piste == 2)
                {
                    plane1.x = 600;
                    plane1.y = 175;
                    //// Lumières debut de piste (verte)
                    lights.xLeftGreen = 514;
                    lights.xRightGreen = 524;
                    lights.yLeftGreen = 214;
                    lights.yRightGreen = 234;

                    // Lumières fin de piste (rouge)
                    lights.xLeftRed = 271;
                    lights.xRightRed = 281;
                    lights.yLeftRed = 350;
                    lights.yRightRed = 370;
                }
                else if (p[nb - 1].piste == 1)
                {
                    plane1.x = 382;
                    plane1.y = 10;
                    //// Lumières debut de piste (verte)
                    lights.xLeftGreen = 375;
                    lights.xRightGreen = 402;
                    lights.yLeftGreen = 110;
                    lights.yRightGreen = 110;
                    // Lumières fin de piste (rouge)
                    lights.xLeftRed = 375;
                    lights.xRightRed = 402;
                    lights.yLeftRed = 480;
                    lights.yRightRed = 480;
                }
                else if (p[nb - 1].piste == 4)
                {
                    plane1.x = 175;
                    plane1.y = 405;
                    //// Lumières debut de piste (verte)
                    lights.xLeftGreen = 271;
                    lights.xRightGreen = 281;
                    lights.yLeftGreen = 350;
                    lights.yRightGreen = 370;
                    // Lumières fin de piste (rouge)
                    lights.xLeftRed = 514;
                    lights.xRightRed = 524;
                    lights.yLeftRed = 214;
                    lights.yRightRed = 234;
                }
                // Cliquer sur la bouton 'Retour'
                if (x >= btns.retourBtn.min_x && x <= btns.retourBtn.max_x &&
                    y >= btns.retourBtn.min_y && y <= btns.retourBtn.max_y)
                {
                    Mix_PlayMusic(buttonSound, 1);
                    eI = UTIL;
                }
            }
            else if (eI == UTIL)
            {
                int x = event.button.x, y = event.button.y;
                // Cliquer sur la bouton 'Quitter'
                if (x >= quitterUtilisateur.x && x <= quitterUtilisateur.x + quitterUtilisateur.w &&
                    y >= quitterUtilisateur.y && y <= quitterUtilisateur.y + quitterUtilisateur.h)
                {
                    Mix_PlayMusic(buttonSound, 1);
                    eI = MENU;
                }
            }
            else if (eI == IDENT)
            {
                int x = event.button.x, y = event.button.y;
                // Cliquer sur le cadre du texte
                if (x >= immatriculationView.x && x <= immatriculationView.x + 200 &&
                    y >= immatriculationView.y && y <= immatriculationView.y + immatriculationView.h)
                {
                    isImmatriculationTexteInput = TRUE;
                    isVitesseTexteInput = FALSE;
                    isCapTexteInput = FALSE;
                    isFuelTexteInput = FALSE;

                    SDL_StartTextInput();
                }
                else if (x >= vitesseView.x && x <= vitesseView.x + 200 &&
                         y >= vitesseView.y && y <= vitesseView.y + vitesseView.h)
                {
                    isVitesseTexteInput = TRUE;
                    isImmatriculationTexteInput = FALSE;
                    isCapTexteInput = FALSE;
                    isFuelTexteInput = FALSE;

                    SDL_StartTextInput();
                }
                else if (x >= capView.x && x <= capView.x + 200 &&
                         y >= capView.y && y <= capView.y + capView.h)
                {
                    isCapTexteInput = TRUE;
                    isImmatriculationTexteInput = FALSE;
                    isVitesseTexteInput = FALSE;
                    isFuelTexteInput = FALSE;

                    SDL_StartTextInput();
                }
                else if (x >= fuelView.x && x <= fuelView.x + 200 &&
                         y >= fuelView.y && y <= fuelView.y + fuelView.h)
                {
                    isFuelTexteInput = TRUE;
                    isImmatriculationTexteInput = FALSE;
                    isVitesseTexteInput = FALSE;
                    isCapTexteInput = FALSE;

                    SDL_StartTextInput();
                }

                else if (x >= terminer.x && x <= terminer.x + terminer.w &&
                         y >= terminer.y && y <= terminer.y + terminer.h)
                {
                    int valCapTextInput = parseInt(capTextInput);
                    if (immatriculationTextLength != 0 && vitesseTextLength != 0 && capTextLength != 0 && fuelTextLength != 0 && valCapTextInput <= 360 && valCapTextInput >= 0)
                    {
                        if (parseInt(fuelTextInput) <= 10 && parseInt(vitesseTextInput) >= 200)
                        {
                            Mix_PlayMusic(alertSound, 1);
                            MessageBox(0, "Fuel bas & Vitesse haute", "Alert", MB_ICONWARNING);
                        }
                        else if (parseInt(fuelTextInput) <= 10)
                        {
                            Mix_PlayMusic(alertSound, 1);
                            MessageBox(0, "Fuel bas \nVous etes prioritaire sur la piste", "Alert", MB_ICONWARNING);
                        }
                        else if (parseInt(vitesseTextInput) >= 200)
                        {
                            Mix_PlayMusic(alertSound, 1);
                            MessageBox(0, "Vitesse haute", "Alert", MB_ICONWARNING);
                        }
                        Mix_PlayMusic(validateSound, 1);

                        isFuelTexteInput = FALSE;
                        isImmatriculationTexteInput = FALSE;
                        isVitesseTexteInput = FALSE;
                        isCapTexteInput = FALSE;
                        int valVitesseTextInput = parseInt(vitesseTextInput);
                        int valFuelTextInput = parseInt(fuelTextInput);
                        strcpy(p[nb].immatriculation, immatriculationTextInput);
                        p[nb].vitesse = valVitesseTextInput;
                        p[nb].cap = valCapTextInput;
                        p[nb].fuel = valFuelTextInput;

                        // Pour affichage dans AFF
                        sprintf(p[nb].liste, "Immatriculation : %s | Vitesse : %s | Cap : %s | Fuel : %s", immatriculationTextInput, vitesseTextInput, capTextInput, fuelTextInput);
                        // Stock les valeurs dans les fichiers log
                        if (nb <= 4)
                        {
                            stock();
                        }
                        // Clear text inputs
                        SDL_DestroyTexture(affichage.immatriculationTexte);
                        strcpy(immatriculationTextInput, " ");
                        immatriculationTextLength = 0;
                        immatriculationTextInput[strlen(immatriculationTextInput) - 1] = 0;
                        SDL_DestroyTexture(affichage.vitesseTexte);
                        strcpy(vitesseTextInput, " ");
                        vitesseTextLength = 0;
                        vitesseTextInput[strlen(vitesseTextInput) - 1] = 0;
                        SDL_DestroyTexture(affichage.capTexte);
                        strcpy(capTextInput, " ");
                        capTextLength = 0;
                        capTextInput[strlen(capTextInput) - 1] = 0;
                        SDL_DestroyTexture(affichage.fuelTexte);
                        strcpy(fuelTextInput, " ");
                        fuelTextLength = 0;
                        fuelTextInput[strlen(fuelTextInput) - 1] = 0;
                        immatriculation.w = vitesse.w = cap.w = fuel.w = 0;
                        nb++;
                        eI = UTIL;
                    }
                    else if (immatriculationTextLength == 0 || vitesseTextLength == 0 || capTextLength == 0 || fuelTextLength == 0)
                    {
                        errorFieldBool = TRUE;
                        Mix_PlayMusic(errorSound2, 1);
                    }
                    /*else if (valCapTextInput > 360 && errorFieldBool == FALSE)
                    {
                        errorCapBool = TRUE;
                    }*/
                }
                else
                {
                    isImmatriculationTexteInput = FALSE;
                    isVitesseTexteInput = FALSE;
                    isCapTexteInput = FALSE;
                    isFuelTexteInput = FALSE;
                    SDL_StopTextInput();
                }

                // Cliquer sur la bouton 'Retour'
                if (x >= btns.retourBtn.min_x && x <= btns.retourBtn.max_x &&
                    y >= btns.retourBtn.min_y && y <= btns.retourBtn.max_y)
                {
                    Mix_PlayMusic(buttonSound, 1);
                    eI = MENU;
                }
            }
            else if (eI == MENU)
            {
                int bouton = mouseABouton(event.button.x, event.button.y, btns);
                if (bouton == 0)
                {
                    Mix_PlayMusic(buttonSound, 1);
                    eI = AFF;
                }
                else if (bouton == 1)
                {
                    if (nb >= 4)
                    {
                        Mix_PlayMusic(errorSound, 1);
                        MessageBox(0, "NOMBRE AERONEFS MAX ATTEINT", "Information", MB_OK);
                    }
                    else
                    {
                        Mix_PlayMusic(buttonSound, 1);
                        eI = IDENT;
                    }
                }
                else if (bouton == 2)
                {
                    eI = END;
                }
            }
            break;

        case SDL_MOUSEMOTION:
            x = event.motion.x;
            y = event.motion.y;
            break;
        case SDL_QUIT:
            eI = EXIT;
            break;
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 70);
        if (isImmatriculationTexteInput)
            SDL_RenderFillRect(renderer, &immatriculationView);
        if (isVitesseTexteInput)
            SDL_RenderFillRect(renderer, &vitesseView);
        if (isCapTexteInput)
            SDL_RenderFillRect(renderer, &capView);
        if (isFuelTexteInput)
            SDL_RenderFillRect(renderer, &fuelView);
        if (errorFieldBool && eI == IDENT && errorCapBool == FALSE && errorFuelBool == FALSE)
        {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 100);
            SDL_RenderFillRect(renderer, &errorField);
            SDL_RenderCopy(renderer, affichage.errorFieldTexte, NULL, &errorField);
            surf.errorFieldSurfaceTexte = TTF_RenderText_Blended(font, "Champs Incomplet", black);
            affichage.errorFieldTexte = SDL_CreateTextureFromSurface(renderer, surf.errorFieldSurfaceTexte);
        }
        if (errorCapBool && eI == IDENT && errorFieldBool == FALSE && errorFuelBool == FALSE)
        {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 100);
            SDL_RenderFillRect(renderer, &errorField);
            SDL_RenderCopy(renderer, affichage.errorFieldTexte, NULL, &errorField);
            surf.errorFieldSurfaceTexte = TTF_RenderText_Blended(font, "Cap invalide", black);
            affichage.errorFieldTexte = SDL_CreateTextureFromSurface(renderer, surf.errorFieldSurfaceTexte);
        }
        if (errorFuelBool && eI == IDENT && errorFieldBool == FALSE && errorCapBool == FALSE)
        {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 100);
            SDL_RenderFillRect(renderer, &errorField);
            SDL_RenderCopy(renderer, affichage.errorFieldTexte, NULL, &errorField);
            surf.errorFieldSurfaceTexte = TTF_RenderText_Blended(font, "Fuel invalide", black);
            affichage.errorFieldTexte = SDL_CreateTextureFromSurface(renderer, surf.errorFieldSurfaceTexte);
        }
        SDL_Rect dstrect = {x, y, 25, 25};
        SDL_RenderCopy(renderer, cursorTexte, NULL, &dstrect);

        SDL_RenderPresent(renderer);
    }
    FreeImages(&images);
    DestroyTexture(&texture);
    FreeSurface(&surface);
    SDL_DestroyWindow(ecran);
    SDL_DestroyRenderer(renderer);
    TTF_CloseFont(font);
    TTF_Quit();
    IMG_Quit();
    // Libération AUDIO
    Mix_FreeMusic(musique);
    Mix_CloseAudio();
    //
    SDL_Quit();
    return EXIT_SUCCESS;
}
void initButtons(Buttons *btns)
{
    short int w = 305, h = 98;
    // (x coords)
    btns->afficherBtn.min_x = btns->identificationBtn.min_x = btns->quitterBtn.min_x = btns->validerBtn.min_x = 248;
    btns->afficherBtn.max_x = btns->identificationBtn.max_x = btns->quitterBtn.max_x = btns->validerBtn.max_x = 248 + w;
    btns->retourBtn.min_x = 0;
    btns->retourBtn.max_x = w / 2;
    btns->carteBtn.min_x = 30;
    btns->carteBtn.max_x = 30 + (w / 2);
    // (y coords)
    btns->identificationBtn.min_y = 206;
    btns->identificationBtn.max_y = 206 + h;
    btns->afficherBtn.min_y = btns->validerBtn.min_y = 334;
    btns->afficherBtn.max_y = btns->validerBtn.max_y = 334 + h;
    btns->quitterBtn.min_y = 461;
    btns->quitterBtn.max_y = 461 + h;
    btns->retourBtn.min_y = 0;
    btns->retourBtn.max_y = h / 2;
    btns->carteBtn.min_y = 353;
    btns->carteBtn.max_y = 353 + (h / 2);
}
void FreeImages(Images *images)
{
    SDL_DestroyTexture(images->menuIMG);
    SDL_DestroyTexture(images->identificationIMG);
    SDL_DestroyTexture(images->afficherIMG);
    SDL_DestroyTexture(images->utilisateurIMG);
    SDL_DestroyTexture(images->carteIMG);
    SDL_DestroyTexture(images->quitterIMG);
}
void DestroyTexture(rectTexture *texture)
{
    SDL_DestroyTexture(texture->immatriculationTexte);
    SDL_DestroyTexture(texture->vitesseTexte);
    SDL_DestroyTexture(texture->capTexte);
    SDL_DestroyTexture(texture->fuelTexte);
    SDL_DestroyTexture(texture->liste1Texte);
    SDL_DestroyTexture(texture->liste2Texte);
    SDL_DestroyTexture(texture->liste3Texte);
    SDL_DestroyTexture(texture->liste4Texte);
    SDL_DestroyTexture(texture->heureRectTexte);
    SDL_DestroyTexture(texture->nbRectTexte);
    SDL_DestroyTexture(texture->immatriculationUtilisateurTexte);
    SDL_DestroyTexture(texture->pisteUtilisateurTexte);
}
void FreeSurface(Surface *surface)
{
    SDL_FreeSurface(surface->immatriculationSurfaceTexte);
    SDL_FreeSurface(surface->vitesseSurfaceTexte);
    SDL_FreeSurface(surface->capSurfaceTexte);
    SDL_FreeSurface(surface->fuelSurfaceTexte);

    SDL_FreeSurface(surface->liste1SurfaceTexte);
    SDL_FreeSurface(surface->liste2SurfaceTexte);
    SDL_FreeSurface(surface->liste3SurfaceTexte);
    SDL_FreeSurface(surface->liste4SurfaceTexte);

    SDL_FreeSurface(surface->heureRectSurfaceTexte);
    SDL_FreeSurface(surface->nbRectSurfaceTexte);
    SDL_FreeSurface(surface->immatriculationUtilisateurSurfaceTexte);
    SDL_FreeSurface(surface->pisteUtilisateurSurfaceTexte);

    SDL_FreeSurface(surface->errorFieldSurfaceTexte);
}

void creerEtAfficherUnRectangle(int x, int y, int h, int w, SDL_Renderer *renderer, SDL_Surface *surface, int r, int g, int b, int t)
{
    SDL_Rect nom;
    nom.x = x;
    nom.y = y;
    nom.h = h;
    nom.w = w;
    SDL_SetRenderDrawColor(renderer, r, g, b, t);
    SDL_RenderFillRect(renderer, &nom);
}

void chargerImages(Images *images, SDL_Renderer *renderer)
{
    SDL_Surface *s = IMG_Load("images/menu1.png");
    images->menuIMG = SDL_CreateTextureFromSurface(renderer, s);
    s = IMG_Load("images/identification.png");
    images->identificationIMG = SDL_CreateTextureFromSurface(renderer, s);
    s = IMG_Load("images/afficher.png");
    images->afficherIMG = SDL_CreateTextureFromSurface(renderer, s);
    s = IMG_Load("images/utilisateur.png");
    images->utilisateurIMG = SDL_CreateTextureFromSurface(renderer, s);
    s = IMG_Load("images/carte.png");
    images->carteIMG = SDL_CreateTextureFromSurface(renderer, s);
    s = IMG_Load("images/quitter.png");
    images->quitterIMG = SDL_CreateTextureFromSurface(renderer, s);
}
int afficherImage(SDL_Renderer *renderer, SDL_Texture *texture)
{
    SDL_Rect dest = {0, 0, 800, 600};
    if (texture == NULL || SDL_RenderCopy(renderer, texture, NULL, &dest) != 0)
    {
        printf("Error : %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
short int mouseABouton(int x, int y, Buttons btns)
{
    short int bouton = -1;
    if (x >= btns.identificationBtn.min_x && x <= btns.identificationBtn.max_x)
    {
        // Cliquer sur la bouton 'afficher'
        if (y >= btns.afficherBtn.min_y && y <= btns.afficherBtn.max_y)
            bouton = 0;
        // Cliquer sur la bouton 'immatriculation'
        else if (y >= btns.identificationBtn.min_y && y <= btns.identificationBtn.max_y)
            bouton = 1;
        // Cliquer sur la bouton 'QUITTER'
        else if (y >= btns.quitterBtn.min_y && y <= btns.quitterBtn.max_y)
            bouton = 2;
    }
    return bouton;
}
int parseInt(char *chars)
{
    int sum = 0;
    int len = strlen(chars);
    for (int x = 0; x < len; x++)
    {
        int n = chars[len - (x + 1)] - '0';
        sum = sum + powInt(n, x);
    }
    return sum;
}
int powInt(int x, int y)
{
    for (int i = 0; i < y; i++)
    {
        x *= 10;
    }
    return x;
}

//----------------  | Fonctions programme interne | ----------------

//--------------------------------------------------------------------------- Fonction stock
void stock()
{
    prise_en_charge();
    time_t t;
    time(&t);
    strcpy(p[nb].date, ctime(&t));
// Stockage pour ecxel
#define TAILLE_MAX 2048
    char sauv[TAILLE_MAX];
    FILE *fu = fopen("fichierLog/Enregistrement_avion.csv", "a");
    if (fu == NULL)
    {
        exit(1);
    }
    //
    if (nb == 0)
    {
        fprintf(fu, "Immatriculation;Vitesse;Cap;Fuel;Piste;Date\n");
    }
    fprintf(fu, "%s;%d;%d;%d;%d;%s", p[nb].immatriculation, p[nb].vitesse, p[nb].cap, p[nb].fuel, p[nb].piste, p[nb].date);
    fclose(fu);
    // FIN DE STOCKAGE
    // Fichier Log
    char sauv2[TAILLE_MAX];
    char aV[20] = {""};
    char aF[20] = {""};
    FILE *fx = fopen("fichierLog/log.csv", "a");
    if (fx == NULL)
    {
        exit(1);
    }
    if (nb == 0)
    {
        fprintf(fx, "Immatriculation;Alerte Vitesse;Alerte Fuel;Date\n");
    }
    if (p[nb].vitesse >= 200)
    {
        strcpy(aV, "Alerte");
    }
    if (p[nb].fuel <= 10)
    {
        strcpy(aF, "Alerte");
    }
    fprintf(fx, "%s;%s;%s;%s", p[nb].immatriculation, aV, aF, p[nb].date);
    fclose(fx);
}
//--------------------------------------------------------------------------- Fonction prise_en_charge
void prise_en_charge()
{
    if (p[nb].cap <= 40)
    {
        p[nb].piste = 3;
    }
    else if (p[nb].cap >= 320)
    {
        p[nb].piste = 3;
    }
    else if (p[nb].cap <= 220 && p[nb].cap >= 140)
    {
        p[nb].piste = 1;
    }
    else if (p[nb].cap < 140 && p[nb].cap > 40)
    {
        p[nb].piste = 4;
    }
    else if (p[nb].cap < 320 && p[nb].cap > 220)
    {
        p[nb].piste = 2;
    }
}
void clear_files()
{
    FILE *fa = fopen("fichierLog/Enregistrement_avion.csv", "w");
    if (fa == NULL)
    {
        exit(1);
    }
    fprintf(fa, "");
    fclose(fa);
    FILE *fx = fopen("fichierLog/log.csv", "w");
    if (fx == NULL)
    {
        exit(1);
    }
    fprintf(fx, "");
    fclose(fx);
}