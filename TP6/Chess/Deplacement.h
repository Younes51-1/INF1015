#pragma once

#include <utility>

namespace modele
{
	class Table;

	class Deplacement
	{
		public:
		virtual void trouverDeplacementsPossibles(Table& Table) = 0;
		virtual ~Deplacement() = default;
	};
}
