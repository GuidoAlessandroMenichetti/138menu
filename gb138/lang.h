#ifndef LANGUAGE_HEADER
#define LANGUAGE_HEADER

/****

Order is always:
	ENGLISH
	GERMAN
	SPANISH
	FRENCH
	TURKISH
	POLISH
	
****/

const char lang_names[][16] =
{
	"English",
	"Deutsch",
	"Espanol",
	"Francais",
	"Turkce",
	"Polski"
};
const char lang_main_menu[][3][20] = 
{	
	{"SETTINGS",		"GAME", 	"INSTALL"},
	{"EINSTELLUNGEN",	"SPIELE",	"INSTALLIEREN"},
	{"OPCIONES",		"JUEGOS", 	"INSTALAR"},
	{"CONFIGURATIONS",	"JEUX", 	"INSTALLATION"},
	{"AYARLAR",			"OYUNLAR",	"YUKLEMELER"},
	{"USTAWIENIA",		"GRY", 		"INSTALATOR"}
};
const char lang_no_apps[][40] =
{
	"No homebrew found in:", 
	"Keine Homebrews gefunden in:",
	"No se encontraron aplicaciones en:",
	"Aucune applications trouve:",
	"Homebrew Bulunamadi:",
	"Nie znaleziono homebrewow w:"
};
const char lang_no_installers[][50] =
{
	"No install files found\0", 
	"Keine Installationsdateien gefunden\0",
	"No se encontraron archivos de instalacion\0",
	"Aucun fichier instable trouve\0",
	"Yukleme Bulunamadi\0",
	"Nie znaleziono plikow instalacyjnych\0"
};
const char lang_msg_delete[][20] =
{
	"Delete ",
	"Loesche ",
	"Borrar ",
	"Effacer ",
	"Sil ",
	"Usun "
};
const char lang_msg_install[][20] =
{
	"Install ",
	"Installiere ",
	"Instalar ",
	"Installer ",
	"Yukle ",
	"Zainstaluj "
};
const char lang_msg_launch[][20] = 
{
	"Launch ",
	"Starte ",
	"Ejecutar ",
	"Lancer ",
	"Baslat ",
	"Launch "
};
const char lang_msg_options[][2][13] =
{
	{"Confirm", 	"Cancel"}, 
	{"Bestaetigen", "Abbrechen"},
	{"Aceptar", 	"Cancelar"},
	{"Confirmer", 	"Annuler"},
	{"Evet", 		"Hayir"},
	{"Potwierdz", 	"Anuluj"}
};
const char lang_settings_yesno[][2][10] = 
{
	{"No", 		"Yes"},
	{"Nein", 	"Ja"},
	{"No", 		"Si"},
	{"Non", 	"Oui"},
	{"Hayir", 	"Evet"},
	{"Nie", 	"Tak"}
};
const char lang_settings_modes[][2][20] =
{
	{"Accurate", "Fast"},
	{"Akkurat", "Schnell"},
	{"Preciso", "Rapido"},
	{"Exact", "Vite"},
	{"Konturollu", "Hizli"},
	{"Dokladny", "Szybki"}
};
const char lang_settings[][11][45] =
{
	{
		"Language", 
		"Load homebrew icon", 
		"Load installer icon", 
		"Installer load mode", 
		"Install as...", 
		"Install to...", 
		"List /GAME/ homebrew", 
		"Theme",
		"Background",
		"Clean Homebrew folder", 
		"Restore defaults", 
	},
	
	{
		"Sprache", 
		"Zeige Homebrew Symbol", 
		"Zeige Installer Symbol", 
		"Installer Lademodus", 
		"Installiere als...", 
		"Installiere nach...", 
		"Liste /GAME/ Homebrews",
		"Theme",
		"Hintergrund",
		"Leere Homebrew Ordner",
		"Einstellungen zuruecksetzen"
	},
	
	{
		"Idioma", 
		"Cargar imagen de aplicacion", 
		"Cargar imagen de instalador", 
		"Modo de carga de instaladores",
		"Instalar como...",
		"Instalar en...", 
		"Listar aplicaciones de /GAME/",
		"Tema",
		"Fondo",
		"Limpiar carpeta de homebrew",
		"Restablecer por defecto", 
	},
	
	{
		"Langue", 
		"Chargez l'icone d'application",
		"Chargez l'icone d'installation", 
		"Installez mode de chargement",
		"Installez comme...",
		"Installez a...",
		"Listez jeux fait maison /GAME/",
		"Theme",
		"Fond d'ecran",
		"Nettoyez dossier Homebrew",
		"Restituez les parametre de defauts"	
	},
	
	{
		"Dil", 
		"Homebrew Resmi Goster", 
		"Yukleme Resmi Goster", 
		"Yukleme Modu",
		"Yukleme ismi...", 
		"Yukleme Yeri...", 
		"/GAME/ Homebrew Goster",
		"Temalar",
		"Arkapilan",
		"Homebrew klasoru temizle",
		"Ayarlari Sifirla"
	},
	
	{
		"Jezyk", 
		"Wczytuj ikony dla homebrewow", 
		"Wczytuj ikony dla instalatorow", 
		"Tryb wczytywania instalatorow", 
		"Zainstaluj jako...", 
		"Zainstaluj w...", 
		"Lista aplikacji z /GAME/", 
		"Styl",
		"Tla",
		"Oczyscic Homebrew folderu",
		"Przywroc ustawienia domyslne", 
	},
};

#endif