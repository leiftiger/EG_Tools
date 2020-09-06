#pragma once

#include <unordered_map>
#include <unordered_set>

#include "ATN_Entry.h"
#include "ATN_Exception.h"

namespace ATN
{
	enum DeltaPatchType : int
	{
		KEEP,
		ADD,
		SUB,
		REM
	};

	template <class T>
	class DeltaPatch
	{
	public:

		// Type of patch
		DeltaPatchType m_type;

		// Index in the original list where the entry should be added/removed from
		size_t m_pos;

		// These aren't references because they'd for some reason be
		// released even though they point to the heap for entries

		// The original object
		const T m_original;
		// The changed object
		const T m_change;

		DeltaPatch(DeltaPatchType type, size_t pos, const T &original, const T &change) : m_type(type), m_pos(pos), m_original(original), m_change(change)
		{

		}

		static void insert(size_t pos, const T &entry, std::vector<T> &list)
		{
			list.push_back(entry);

			// If not inserted in the back, move all affected elements
			if (pos != (list.size() - 1))
			{
				for (size_t i = list.size() - 1; i > pos; i--)
				{
					list[i] = list[i - 1];
				}

				list[pos] = entry;
			}
		}

		static void remove(size_t pos, std::vector<T> &list)
		{
			for (size_t i = pos; i < list.size() - 1; i++)
			{
				list[i] = list[i + 1];
			}

			list.pop_back();
		}
	};

	// Helper class to maintain global changed references for delta patching
	class DeltaMemory
	{
	protected:

		std::unordered_map<std::string, std::vector<int>> m_translations;
		std::unordered_set<std::string> m_lockedTranslations;

	public:

		std::vector<int> &translations(const std::string &type);

		// Returns true if the translations for the specific type shouldn't be used
		bool isLocked(const std::string &type) const;
		// Prevents the specific translation list from being used (due to e.g. external changes)
		void lock(const std::string &type);

		// Returns true if the specific type has been modified
		bool isChanged(const std::string &type) const;

		// Updates all references after the index by diff to compensate for recent additions or removals
		void updateTranslationsAfter(const std::string &type, int index, int diff);
	};

	// Creates a number of patches representing the difference between the original and changed list
	template <class T>
	std::vector<DeltaPatch<T>> deltaDifferences(const std::vector<T> &original, const std::vector<T> &change)
	{
		std::vector<DeltaPatch<T>> patches;

		int **distance = new int*[original.size() + 1];

		for (std::size_t i = 0; i <= original.size(); i++)
		{
			distance[i] = new int[change.size() + 1];
		}

		int **operation = new int*[original.size() + 1];

		for (std::size_t i = 0; i <= original.size(); i++)
		{
			operation[i] = new int[change.size() + 1];
		}

		for (std::size_t i = 1; i <= original.size(); i++)
		{
			distance[i][0] = i;
			operation[i][0] = ADD;
		}

		for (std::size_t i = 1; i <= change.size(); i++)
		{
			distance[0][i] = i;
			operation[0][i] = REM;
		}

		distance[0][0] = 0;
		operation[0][0] = KEEP;

		// We calculate edit distance for each list to see what was changed
		for (std::size_t iOriginal = 1; iOriginal <= original.size(); iOriginal++)
		{
			for (std::size_t iChange = 1; iChange <= change.size(); iChange++)
			{
				const T &originalEntry = original[iOriginal - 1];
				const T &changeEntry = change[iChange - 1];

				int dist = distance[iOriginal - 1][iChange - 1] + 2;

				int op = SUB;

				if (originalEntry == changeEntry)
				{
					dist -= 2;
					op = KEEP;
				}

				if ((distance[iOriginal - 1][iChange] + 1) < dist)
				{
					dist = distance[iOriginal - 1][iChange] + 1;
					op = REM;
				}

				if ((distance[iOriginal][iChange - 1] + 1) < dist)
				{
					dist = distance[iOriginal][iChange - 1] + 1;
					op = ADD;
				}

				distance[iOriginal][iChange] = dist;
				operation[iOriginal][iChange] = op;
			}
		}

		int iOriginal = (int)original.size() - 1, iChange = (int)change.size() - 1;

		while (iOriginal >= 0 && iChange >= 0)
		{
			const T &originalEntry = original[iOriginal];
			const T &changeEntry = change[iChange];

			int op = operation[iOriginal + 1][iChange + 1];

			switch (op)
			{
			case KEEP:
			{
				iOriginal--;
				iChange--;
			}
			break;
			case SUB:
			{
				patches.push_back(DeltaPatch<T>(SUB, iOriginal, originalEntry, changeEntry));

				iOriginal--;
				iChange--;
			}
			break;
			case ADD:
			{
				patches.push_back(DeltaPatch<T>(ADD, iOriginal, originalEntry, changeEntry));

				iChange--;
			}
			break;
			case REM:
			{
				patches.push_back(DeltaPatch<T>(REM, iOriginal, originalEntry, changeEntry));

				iOriginal--;
			}
			break;
			}
		}

		if (iOriginal != -1)
		{
			for (int i = iOriginal; i > -1; i--)
			{
				const T &originalEntry = original[i];

				patches.push_back(DeltaPatch<T>(REM, i, originalEntry, originalEntry));
			}
		}

		if (iChange != -1)
		{
			for (int i = iChange; i > -1; i--)
			{
				const T &changeEntry = change[i];

				patches.push_back(DeltaPatch<T>(ADD, 0, changeEntry, changeEntry));
			}
		}

		for (std::size_t i = 0; i <= original.size(); i++)
		{
			delete[] distance[i];
		}

		for (std::size_t i = 0; i <= original.size(); i++)
		{
			delete[] operation[i];
		}

		delete[] distance;
		delete[] operation;

		return patches;
	}

	// Applies a set of patches to the current list
	template <class T>
	void deltaApply(const std::vector<DeltaPatch<T>> &deltaPatches, std::vector<T> &list, DeltaMemory &memory, const std::string &memoryType)
	{
		std::vector<int> &baseTranslations = memory.translations(memoryType);

		// If this is the first occurence for this file then we can safely assume the current state is unmodified
		if (baseTranslations.size() == 0)
		{
			for (std::size_t i = 0; i < list.size(); i++)
			{
				baseTranslations.push_back(i);
			}

			// If the base list is completely empty, then we set this so new additions get placed at index 0
			if (baseTranslations.size() == 0)
				baseTranslations.push_back(-1);
		}

		for (const DeltaPatch<T> &patch : deltaPatches)
		{
			int curIndex = baseTranslations[patch.m_pos];

			if (curIndex == -2)
			{
				continue;
			}

			switch (patch.m_type)
			{
			case KEEP:
			{
				// NOOP
			}
			break;
			case ADD:
			{
				patch.insert(curIndex + 1, patch.m_change, list);

				memory.updateTranslationsAfter(memoryType, curIndex, 1);
			}
			break;
			case REM:
			{
				if (list[curIndex] == patch.m_original)
				{
					patch.remove(curIndex, list);

					baseTranslations[patch.m_pos] = -2;

					memory.updateTranslationsAfter(memoryType, curIndex, -1);
				}
			}
			break;
			case SUB:
			{
				if (list[curIndex] == patch.m_original)
				{
					list[curIndex] = patch.m_change;
				}
			}
			break;
			}
		}
	}

	// Creates delta patches between original and change, applying them to the current list
	template <class T>
	void deltaUpdate(const std::vector<T> &original, const std::vector<T> &change, std::vector<T> &current, DeltaMemory &memory, const std::string &memoryType)
	{
		std::vector<DeltaPatch<T>> patches = deltaDifferences(original, change);

		deltaApply(patches, current, memory, memoryType);
	}

	// Different functions for entries

	// Creates a number of patches representing the difference between the original and changed list
	template <class T>
	std::vector<DeltaPatch<T*>> deltaDifferencesID(const std::vector<T*> &original, const std::vector<T*> &change)
	{
		std::vector<DeltaPatch<T*>> patches;

		int **distance = new int*[original.size() + 1];

		for (std::size_t i = 0; i <= original.size(); i++)
		{
			distance[i] = new int[change.size() + 1];
		}

		int **operation = new int*[original.size() + 1];

		for (std::size_t i = 0; i <= original.size(); i++)
		{
			operation[i] = new int[change.size() + 1];
		}

		for (std::size_t i = 1; i <= original.size(); i++)
		{
			distance[i][0] = i;
			operation[i][0] = ADD;
		}

		for (std::size_t i = 1; i <= change.size(); i++)
		{
			distance[0][i] = i;
			operation[0][i] = REM;
		}

		distance[0][0] = 0;
		operation[0][0] = KEEP;

		// We calculate edit distance for each list to see what was changed
		for (std::size_t iOriginal = 1; iOriginal <= original.size(); iOriginal++)
		{
			for (std::size_t iChange = 1; iChange <= change.size(); iChange++)
			{
				T *originalEntry = original[iOriginal - 1];
				T* changeEntry = change[iChange - 1];

				int dist = distance[iOriginal - 1][iChange - 1] + 2;

				int op = SUB;

				if (originalEntry->id() == changeEntry->id())
				{
					dist -= 2;
					op = KEEP;
				}

				if ((distance[iOriginal - 1][iChange] + 1) < dist)
				{
					dist = distance[iOriginal - 1][iChange] + 1;
					op = REM;
				}

				if ((distance[iOriginal][iChange - 1] + 1) < dist)
				{
					dist = distance[iOriginal][iChange - 1] + 1;
					op = ADD;
				}

				distance[iOriginal][iChange] = dist;
				operation[iOriginal][iChange] = op;
			}
		}

		int iOriginal = (int)original.size() - 1, iChange = (int)change.size() - 1;

		while (iOriginal >= 0 && iChange >= 0)
		{
			T *originalEntry = original[iOriginal];
			T *changeEntry = change[iChange];

			int op = operation[iOriginal + 1][iChange + 1];

			switch (op)
			{
			case KEEP:
			{
				iOriginal--;
				iChange--;
			}
			break;
			case SUB:
			{
				patches.push_back(DeltaPatch<T*>(SUB, iOriginal, originalEntry, changeEntry));

				iOriginal--;
				iChange--;
			}
			break;
			case ADD:
			{
				patches.push_back(DeltaPatch<T*>(ADD, iOriginal, originalEntry, changeEntry));

				iChange--;
			}
			break;
			case REM:
			{
				patches.push_back(DeltaPatch<T*>(REM, iOriginal, originalEntry, changeEntry));

				iOriginal--;
			}
			break;
			}
		}

		if (iOriginal != -1)
		{
			for (int i = iOriginal; i > -1; i--)
			{
				T *originalEntry = original[i];

				patches.push_back(DeltaPatch<T*>(REM, i, originalEntry, originalEntry));
			}
		}

		if (iChange != -1)
		{
			for (int i = iChange; i > -1; i--)
			{
				T *changeEntry = change[i];

				patches.push_back(DeltaPatch<T*>(ADD, 0, changeEntry, changeEntry));
			}
		}

		for (std::size_t i = 0; i <= original.size(); i++)
		{
			delete[] distance[i];
		}

		for (std::size_t i = 0; i <= original.size(); i++)
		{
			delete[] operation[i];
		}

		delete[] distance;
		delete[] operation;

		return patches;
	}

	// Applies a set of patches to the current list
	template <class T>
	void deltaApplyID(const std::vector<DeltaPatch<T*>> &deltaPatches, std::vector<T*> &list, DeltaMemory &memory, const std::string &memoryType)
	{
		std::vector<int> &baseTranslations = memory.translations(memoryType);

		// If this is the first occurence for this file then we can safely assume the current state is unmodified
		if (baseTranslations.size() == 0)
		{
			for (std::size_t i = 0; i < list.size(); i++)
			{
				baseTranslations.push_back(i);
			}

			// If the base list is completely empty, then we set this so new additions get placed at index 0
			if (baseTranslations.size() == 0)
				baseTranslations.push_back(-1);
		}

		for (const DeltaPatch<T*> &patch : deltaPatches)
		{
			int curIndex = baseTranslations[patch.m_pos];

			if (curIndex == -2)
			{
				continue;
			}

			switch (patch.m_type)
			{
			case KEEP:
			{
				// NOOP
			}
			break;
			case ADD:
			{
				patch.insert(curIndex + 1, patch.m_change, list);

				memory.updateTranslationsAfter(memoryType, curIndex, 1);
			}
			break;
			case REM:
			{
				if (list[curIndex]->id() == patch.m_original->id())
				{
					patch.remove(curIndex, list);

					baseTranslations[patch.m_pos] = -2;

					memory.updateTranslationsAfter(memoryType, curIndex, -1);
				}
			}
			break;
			case SUB:
			{
				if (list[curIndex]->id() == patch.m_original->id())
				{
					list[curIndex] = patch.m_change;
				}
			}
			break;
			}
		}
	}

	// Creates delta patches between original and change, applying them to the current list
	template <class T>
	void deltaUpdateID(const std::vector<T*> &original, const std::vector<T*> &change, std::vector<T*> &current, DeltaMemory &memory, const std::string &memoryType)
	{
		std::vector<DeltaPatch<T*>> patches = deltaDifferencesID(original, change);

		deltaApplyID(patches, current, memory, memoryType);
	}

	// Different functions for heap-based types

	// Creates a number of patches representing the difference between the original and changed list
	template <class T>
	std::vector<DeltaPatch<T*>> deltaDifferencesMemory(const std::vector<T*> &original, const std::vector<T*> &change)
	{
		std::vector<DeltaPatch<T*>> patches;

		int **distance = new int*[original.size() + 1];

		for (std::size_t i = 0; i <= original.size(); i++)
		{
			distance[i] = new int[change.size() + 1];
		}

		int **operation = new int*[original.size() + 1];

		for (std::size_t i = 0; i <= original.size(); i++)
		{
			operation[i] = new int[change.size() + 1];
		}

		for (std::size_t i = 1; i <= original.size(); i++)
		{
			distance[i][0] = i;
			operation[i][0] = ADD;
		}

		for (std::size_t i = 1; i <= change.size(); i++)
		{
			distance[0][i] = i;
			operation[0][i] = REM;
		}

		distance[0][0] = 0;
		operation[0][0] = KEEP;

		// We calculate edit distance for each list to see what was changed
		for (std::size_t iOriginal = 1; iOriginal <= original.size(); iOriginal++)
		{
			for (std::size_t iChange = 1; iChange <= change.size(); iChange++)
			{
				T *originalEntry = original[iOriginal - 1];
				T* changeEntry = change[iChange - 1];

				int dist = distance[iOriginal - 1][iChange - 1] + 2;

				int op = SUB;

				if (*originalEntry == *changeEntry)
				{
					dist -= 2;
					op = KEEP;
				}

				if ((distance[iOriginal - 1][iChange] + 1) < dist)
				{
					dist = distance[iOriginal - 1][iChange] + 1;
					op = REM;
				}

				if ((distance[iOriginal][iChange - 1] + 1) < dist)
				{
					dist = distance[iOriginal][iChange - 1] + 1;
					op = ADD;
				}

				distance[iOriginal][iChange] = dist;
				operation[iOriginal][iChange] = op;
			}
		}

		int iOriginal = (int)original.size() - 1, iChange = (int)change.size() - 1;

		while (iOriginal >= 0 && iChange >= 0)
		{
			T *originalEntry = original[iOriginal];
			T *changeEntry = change[iChange];

			int op = operation[iOriginal + 1][iChange + 1];

			switch (op)
			{
			case KEEP:
			{
				iOriginal--;
				iChange--;
			}
			break;
			case SUB:
			{
				patches.push_back(DeltaPatch<T*>(SUB, iOriginal, originalEntry, changeEntry));

				iOriginal--;
				iChange--;
			}
			break;
			case ADD:
			{
				patches.push_back(DeltaPatch<T*>(ADD, iOriginal, originalEntry, changeEntry));

				iChange--;
			}
			break;
			case REM:
			{
				patches.push_back(DeltaPatch<T*>(REM, iOriginal, originalEntry, changeEntry));

				iOriginal--;
			}
			break;
			}
		}

		if (iOriginal != -1)
		{
			for (int i = iOriginal; i > -1; i--)
			{
				T *originalEntry = original[i];

				patches.push_back(DeltaPatch<T*>(REM, i, originalEntry, originalEntry));
			}
		}

		if (iChange != -1)
		{
			for (int i = iChange; i > -1; i--)
			{
				T *changeEntry = change[i];

				patches.push_back(DeltaPatch<T*>(ADD, 0, changeEntry, changeEntry));
			}
		}

		for (std::size_t i = 0; i <= original.size(); i++)
		{
			delete[] distance[i];
		}

		for (std::size_t i = 0; i <= original.size(); i++)
		{
			delete[] operation[i];
		}

		delete[] distance;
		delete[] operation;

		return patches;
	}

	// Applies a set of patches to the current list
	template <class T>
	void deltaApplyMemory(const std::vector<DeltaPatch<T*>> &deltaPatches, std::vector<T*> &list, DeltaMemory &memory, const std::string &memoryType)
	{
		std::vector<int> &baseTranslations = memory.translations(memoryType);

		// If this is the first occurence for this file then we can safely assume the current state is unmodified
		if (baseTranslations.size() == 0)
		{
			for (std::size_t i = 0; i < list.size(); i++)
			{
				baseTranslations.push_back(i);
			}

			// If the base list is completely empty, then we set this so new additions get placed at index 0
			if (baseTranslations.size() == 0)
				baseTranslations.push_back(-1);
		}

		for (const DeltaPatch<T*> &patch : deltaPatches)
		{
			int curIndex = baseTranslations[patch.m_pos];

			// Here we use -1 for empty lists as that works with the remaining add functionality
			// so -2 now represents deleted base entries
			if (curIndex == -2)
			{
				continue;
			}

			switch (patch.m_type)
			{
			case KEEP:
			{
				// NOOP
			}
			break;
			case ADD:
			{
				patch.insert(curIndex + 1, new T(*patch.m_change), list);

				memory.updateTranslationsAfter(memoryType, curIndex, 1);
			}
			break;
			case REM:
			{
				if (*list[curIndex] == *patch.m_original)
				{
					delete list[curIndex];

					patch.remove(curIndex, list);

					baseTranslations[patch.m_pos] = -2;

					memory.updateTranslationsAfter(memoryType, curIndex, -1);
				}
			}
			break;
			case SUB:
			{
				if (*list[curIndex] == *patch.m_original)
				{
					delete list[curIndex];

					list[curIndex] = new T(*patch.m_change);
				}
			}
			break;
			}
		}
	}

	// Creates delta patches between original and change, applying them to the current list
	template <class T>
	void deltaUpdateMemory(const std::vector<T*> &original, const std::vector<T*> &change, std::vector<T*> &current, DeltaMemory &memory, const std::string &memoryType)
	{
		std::vector<DeltaPatch<T*>> patches = deltaDifferencesMemory(original, change);

		deltaApplyMemory(patches, current, memory, memoryType);
	}
}