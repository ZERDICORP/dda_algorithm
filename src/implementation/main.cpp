#include "config.h"
#include "tools.h"

float getDistanceToFirstVerticalIntersection(int iPX, int iMX, float fCosAlpha, int iSqW)
{
	int iAX = fCosAlpha >= 0 ? iMX + iSqW : iMX;
	return float((iAX - iPX) / fCosAlpha);
}

float getDistanceToFirstHorizontalIntersection(int iPY, int iMY, float fSinAlpha, int iSqW)
{
	int iAY = fSinAlpha >= 0 ? iMY + iSqW : iMY;
	return float((iAY - iPY) / fSinAlpha);
}

int loop(sf::RenderWindow& window, std::map<std::string, float>& cfg)
{
	bool bNeedToUpdateConsole = true;

	std::string sMap;
	sMap += "__________";
	sMap += "__________";
	sMap += "__##___#__";
	sMap += "__#____#__";
	sMap += "__________";
	sMap += "__________";
	sMap += "________#_";
	sMap += "__#___###_";
	sMap += "__________";
	sMap += "__________";

	int iSqW = 40;
	int iRows = 10;
	int iCols = 10;

	float fAngle = 0;
	float fFov = zer::athm::toRadians(cfg["fov"]);
	float fFovHalf = fFov / 2;
	float fRaysCount = cfg["raysCount"];
	float fRayStep = fFov / fRaysCount;

	sf::Vector2f center({mWH / 2, mWW / 2});

	sf::VertexArray ray(sf::LineStrip, 2);
	ray[0].color = sf::Color(255, 0, 0);
	ray[1].color = sf::Color(255, 0, 0);

	sf::RectangleShape rect(sf::Vector2f(iSqW, iSqW));
	rect.setFillColor(sf::Color(255, 255, 255));

	sf::VertexArray line(sf::LineStrip, 2);
	line[0].color = sf::Color(255, 0, 255);
	line[1].color = sf::Color(255, 0, 255);

	while (window.isOpen())
	{
		fAngle += zer::athm::toRadians(0.3);
		
		window.clear();

		for (int r = 0; r < iRows; ++r)
		{
			line[0].position = sf::Vector2f(0, r * iSqW);
			line[1].position = sf::Vector2f(mWW, r * iSqW);
		
			window.draw(line);

			for (int c = 0; c < iCols; ++c)
			{
				line[0].position = sf::Vector2f(c * iSqW, 0);
				line[1].position = sf::Vector2f(c * iSqW, mWH);
		
				window.draw(line);

				if (sMap[iCols * r + c] == '#')
				{
					rect.setPosition(c * iSqW, r * iSqW);
					
					window.draw(rect);
				}
			}
		}

		/*
			DDA algorithm body.
		*/
		for (float f = -fFovHalf; f < fFovHalf; f += fRayStep)
		{
			float fSinAlpha = sin(fAngle + f);
			float fCosAlpha = cos(fAngle + f);

			int iMY = int(center.y / iSqW) * iSqW;
			int iMX = int(center.x / iSqW) * iSqW;

			float fHorizontal = getDistanceToFirstHorizontalIntersection(center.y, iMY, fSinAlpha, iSqW);
			float fVertical = getDistanceToFirstVerticalIntersection(center.x, iMX, fCosAlpha, iSqW);

			int iSqWY = fSinAlpha >= 0 ? iSqW : -iSqW;
			int iSqWX = fCosAlpha >= 0 ? iSqW : -iSqW;

			for (int i = 0; i < iRows; ++i)
			{
				int iRow = abs((center.y + fSinAlpha * fHorizontal + zer::athm::sign(iSqWY)) / iSqWY);
				int iCol = abs((center.x + fCosAlpha * fHorizontal) / iSqWX);

				if (!zer::athm::inRange2D(iRows, iCols, iRow, iCol) || sMap[iCols * iRow + iCol] == '#')
					break;
			
				fHorizontal += iSqWY / fSinAlpha;
			}

			for (int i = 0; i < iCols; ++i)
			{
				int iRow = abs((center.y + fSinAlpha * fVertical) / iSqWY);
				int iCol = abs((center.x + fCosAlpha * fVertical + zer::athm::sign(iSqWX)) / iSqWX);

				if (!zer::athm::inRange2D(iRows, iCols, iRow, iCol) || sMap[iCols * iRow + iCol] == '#')
					break;
			
				fVertical += iSqWX / fCosAlpha;
			}

			float fDist = fHorizontal < fVertical ? fHorizontal : fVertical;

			ray[0].position = sf::Vector2f(center.x, center.y);
			ray[1].position = sf::Vector2f(center.x + cos(fAngle + f) * fDist, center.y + sin(fAngle + f) * fDist);

			window.draw(ray);
		}
		
		sf::CircleShape player(4);
		player.setPosition(center.x - 4, center.y - 4);
		player.setFillColor(sf::Color(255, 255, 255));

		window.draw(player);

		window.display();

		if (bNeedToUpdateConsole)
		{
			displayConsoleInformation(cfg);
			bNeedToUpdateConsole = false;
		}

		switch (eventListener(window))
		{
			case (CLOSE_EVENT_CODE):
				window.close();
				break;

			case RESTART_EVENT_CODE:
				init(window);
				break;
		}
	}
	return 0;
}

int init(sf::RenderWindow& window)
{
	std::map<std::string, float> cfg = readConfig(mConfigPath);
	return loop(window, cfg);
}