#include <iostream>
#include <opencv2/opencv.hpp>
#include <bitset>

/*
// Fonction de calcul du seuil
int threshold(cv::Mat img)
{
	// Initialisation des min/max
	int max = -1;
	int min = 256;
	// On parcourt chaque pixel
	for (int j = 0; j < img.rows; j++)
	{
		for (int i = 0; i < img.cols; i++)
		{
			// Si l'intensité est supérieure au max, elle devient le nouveau max
			if (img.at<uchar>(j, i) > max) { max = img.at<uchar>(j, i); }
			// Et inversement pour le min
			else if (img.at<uchar>(j, i) < min) { min = img.at<uchar>(j, i); }
		}
	}
	return (max - min) / 2; // On retourne le calcul du seuil
}
*/

// Fonction de binarisation sans vote majoritaire
void global_binarisation(cv::Mat img)
{
	int Th = threshold(img); // On récupère le seuil
	// On parcourt chaque pixel
	for (int j = 0; j < img.rows; j++)
	{
		for (int i = 0; i < img.cols; i++)
		{
			// Si l'intensité est supérieure ou égale au seuil, on met le pixel en blanc
			if (img.at<uchar>(j, i) >= Th) { img.at<uchar>(j, i) = 255; }
			// Et inversement si elle est inférieure au seuil
			else if (img.at<uchar>(j, i) < Th) { img.at<uchar>(j, i) = 0; }
		}
	}
	cv::namedWindow("img", cv::WINDOW_NORMAL);
	imshow("img", img);
	imwrite("global_binarisation.jpg", img);
}

// Fonction de binarisation avec vote majoritaire
void global_binarisation2(cv::Mat img)
{
	int Th = threshold(img); // On récupère le seuil
	int k = img.rows / 25; // On initialise k (largeur de chaque module)
	int cpt = 0; // Variable compteur pour compter le nombre de pixel noir
	// Pour chaque colonne de modules
	for (int col = 0; col < 25; col++)
	{
		// Pour chaque ligne de modules
		for (int lin = 0; lin < 25; lin++)
		{
			// On parcourt les k*k pixels du module
			for (int j = 0; j < k; j++)
			{
				for (int i = 0; i < k; i++)
				{
					// Si son intensité est inférieure au seuil, on la met à 0 puis on incrémente le compteur
					if (img.at<uchar>(lin * k + j, col * k + i) < Th) { img.at<uchar>(lin * k + j, col * k + i) = 0; cpt++; }
				}
			}
			// Si au moins 50% des pixels sont noirs
			// (On pourrait passer à 25% pour éviter la perte de modules noirs mais on perdrait des modules blancs
			if (cpt > k * k / 2)
			{
				// On met tous les pixels du module en noir
				for (int j = 0; j < k; j++)
				{
					for (int i = 0; i < k; i++)
					{
						img.at<uchar>(lin * k + j, col * k + i) = 0;
					}
				}
			}
			else
			{
				// On met tous les pixels du module en blanc
				for (int j = 0; j < k; j++)
				{
					for (int i = 0; i < k; i++)
					{
						img.at<uchar>(lin * k + j, col * k + i) = 255;
					}
				}
			}
			cpt = 0; // Réinitialisation du compteur (entre chaque module)
		}
	}
	cv::namedWindow("img", cv::WINDOW_NORMAL);
	imshow("img", img);
	imwrite("global_binarisation2.jpg", img);
}

// Fonction de test pour ajouter un module supplémentaire pour une colonne et une ligne données
void extra_module(cv::Mat img, int col, int lin)
{
	int k = img.rows / 25; // On initialise k (largeur de chaque module)
	int maxW = k * k / 8; // On définit un nombre max de pixels blanc (ici, 1/8 du module)
	int nbW = 0; // Compteur de pixels blancs, utile si jamais le maxW est impair (la double boucle for forme forcément un carré et peut dépasser le max)
	// On forme un carré de pixels blancs qui commence au quart du module
	for (int j = k / 4; j < (k / 4) + sqrt(maxW); j++)
	{
		for (int i = k / 4; i < (k / 4) + sqrt(maxW); i++)
		{
			img.at<uchar>(lin * k + j, col * k + i) = 255; // On change le pixel en blanc
			nbW++; // On incrémente le compteur de pixels blancs
			if (nbW >= maxW) { break; } // Si jamais on a dépassé le max, on quitte la boucle
		}
		if (nbW >= maxW) { break; } // Deuxième break pour sortir de la 2e boucle for
	}
	cv::namedWindow("img", cv::WINDOW_NORMAL);
	imshow("img", img);
	imwrite("extra_module.jpg", img);
}

// Fonction pour agrandir l'image qr_bin.jpg
cv::Mat enlarge(cv::Mat img, int rows, int cols)
{
	cv::Mat newImg = cv::Mat(rows, cols, cv::IMREAD_GRAYSCALE);
	// Coefficients pour passer d'une taille à l'autre
	int kR = rows / img.rows;
	int kC = cols / img.cols;
	// Pour chaque colonne de l'image d'origine
	for (int col = 0; col < img.cols; col++)
	{
		// Pour chaque ligne de l'image d'origine
		for (int lin = 0; lin < img.rows; lin++)
		{
			// On duplique chaque pixel kR ou kC fois
			for (int j = lin*kR; j < (lin+1)*kR; j++)
			{
				for (int i = col * kC; i < (col + 1) * kC; i++)
				{
					// Petite vérification avec un seuil à 120 puisque sans on avait une image avec
					// parfois des pixel d'intensité 2 ou 253
					if (img.at<uchar>(lin, col) > 120) { newImg.at<uchar>(j, i) = 255;  }
					else { newImg.at<uchar>(j, i) = 0; }
				}
			}
		}
	}
	return newImg;
}

// Fonction de conversion du message en suite de bits
std::string stringToBinary(std::string message)
{
	std::string ret = "";
	// Convert the message in binary
	for (int i = 0; i < message.length(); ++i)
	{
		int ascii = (uchar)message[i];
		std::string byte = std::bitset<8>(ascii).to_string();
		ret += byte;
	}
	return ret;
}

// Fonction pour stocker un message dans le code QR
void extra_storage(cv::Mat img, std::string message)
{
	int cursor = 0; // Curseur utilisé pour situer à quel bit on est
	std::string binary = stringToBinary(message); // On convertit le message en binaire
	int k = img.rows / 25; // On initialise k (largeur de chaque module)
	int maxW = k * k / 8; // On définit un nombre max de pixels blanc (ici, 1/8 du module)
	int nbW = 0; // Compteur de pixels blancs, utile si jamais le maxW est impair (la double boucle for forme forcément un carré et peut dépasser le max)
	// Pour chaque ligne de modules
	for (int lin = 0; lin < 25; lin++)
	{
		// Pour chaque colonne de modules
		for (int col = 0; col < 25; col++)
		{
			// Si le module est noir
			if (img.at<uchar>(lin*k+1, col*k+1) == 0)
			{
				// Si le bit actuel est à 1
				if (binary[cursor] == '1')
				{
					// Même fonction de dessin que dans la fonction extra_module
					for (int j = k / 4; j < (k / 4) + sqrt(maxW); j++)
					{
						for (int i = k / 4; i < (k / 4) + sqrt(maxW); i++)
						{
							img.at<uchar>(lin * k + j, col * k + i) = 255;
							nbW++;
							if (nbW >= maxW) { break; }
						}
						if (nbW >= maxW) { break; }
					}
				}
				cursor++; // On avance le curseur de 1
				nbW = 0; // On réinitialise le nombre actuel de pixels blancs (entre chaque module)
			}
			if (cursor >= binary.length()) { break; } // On quitte la boucle si jamais on a atteint la fin du message
		}
		if (cursor >= binary.length()) { break; } // On quitte la boucle si jamais on a atteint la fin du message
	}
	cv::namedWindow("img", cv::WINDOW_NORMAL);
	imshow("img", img);
	imwrite("enrichi.png", img);
}

// Fonction de décodage d'un message stocké dans un QR code
std::string decode(cv::Mat img)
{
	int k = img.rows / 25; // On initialise k (largeur de chaque module)
	int nbW = 0; // Compteur du nombre de pixel blanc de chaque module
	int nbB = 0; // Compteur du nombre de pixel noir de chaque module
	std::string binary = ""; // Message en binaire
	std::string message = ""; // Message en français
	// Pour chaque colonne de modules
	for (int col = 0; col < 25; col++)
	{
		// Pour chaque ligne de modules
		for (int lin = 0; lin < 25; lin++)
		{
			// On incrémente les compteurs en fonction du pixel consulté
			for (int j = 0; j < k; j++)
			{
				for (int i = 0; i < k; i++)
				{
					if (img.at<uchar>(lin * k + j, col * k + i) == 0) { nbB++; }
					else { nbW++; }
				}
			}
			// Si on a aucun pixel blanc, c'est un bit à 0
			if (nbW == 0) { binary += '0'; }
			// Si on a moins d'un quart de pixel blanc, c'est un bit à 1
			else if (nbW <= k * k / 4) { binary += '1'; }
			// On réinitialise les compteurs
			nbB = 0;
			nbW = 0;
		}
	}
	// Décodage du message binaire en français
	for (int pos = 0; pos < binary.length(); pos += 8)
	{
		std::string byte = binary.substr(pos, 8);
		int dec = std::bitset<8>(byte).to_ulong();
		message += char(dec);
	}
	return message;
}

/*
int main()
{
	cv::Mat img = cv::imread("qr_gray.jpg", cv::IMREAD_GRAYSCALE);
	cv::Mat img2 = cv::imread("qr_bin.jpg", cv::IMREAD_GRAYSCALE);
	cv::Mat img3 = enlarge(img2, 500, 500);
	cv::Mat img4 = cv::imread("qr_rich.png", cv::IMREAD_GRAYSCALE);

	// Exercice 1.3
	//global_binarisation(img);

	// Exercice 1.4
	//global_binarisation2(img);

	// Exercice 2.2
	//extra_module(img3, 6, 3);

	// Exercice 2.4
	//extra_storage(img3, "hello world");

	// Exercice 2.6
	std::cout << decode(img4) << std::endl;

	//cv::namedWindow("img", cv::WINDOW_NORMAL);
	//imshow("img", img4);
	cv::waitKey(0);
	return 0;
}
*/