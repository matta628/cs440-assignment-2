#ifndef MAP_H
#define MAP_H

#include <cstddef>
#include <vector>
#include <list>
#include <utility>
#include <random>
#include <iostream>
#include <ctime>

namespace cs540{

	template <typename Key_T, typename Mapped_T>
	class Map{
	private:
		typedef std::pair<const Key_T, Mapped_T> ValueType;

		class SkipList{
		public:
			struct Node{
				Node() : pred(nullptr), suc(nullptr), above(nullptr), below(nullptr) {}
			    Node *pred; // Predecessor
			    Node *suc; // Successor
				Node *above;
				Node *below;
			};
			struct DataNode : public Node{
				DataNode(ValueType i) : val(i) {}
			    ValueType val;
			};

			SkipList(){
				std::srand(std::time(nullptr));
				height = 1;
				size = 0;
				sentinel = new Node();
				sentinel->suc = sentinel->pred = sentinel;
			}

			~SkipList(){
				Node *next_sent = sentinel;
				Node *curr;
				Node *prev;
				while (next_sent != nullptr){
					curr = next_sent->suc;
					while (curr != next_sent){
						prev = curr;
						curr = curr->suc;
						delete prev;
					}
					prev = next_sent;
					next_sent = next_sent->below;
					delete prev;
				}
			}

			DataNode *find(const Key_T &key) const{
				Node *curr = sentinel;
				Node *curr_sent = sentinel;
				while (curr != nullptr){
					while (curr->suc != curr_sent && static_cast<DataNode *>(curr->suc)->val.first < key){
						curr = curr->suc;
					}
					if (curr->suc != curr_sent && static_cast<DataNode *>(curr->suc)->val.first == key){
						curr = curr->suc;
						//bottomost node holds true Mapped_T object, upper nodes just for key lookup
						while (curr->below != nullptr){
							curr = curr->below;
						}
						return static_cast<DataNode *>(curr);
					}
					curr = curr->below;
					curr_sent = curr_sent->below;
				}

				return nullptr;
			}

			std::pair<Node *, bool> insert(const ValueType &val){

				Node *curr = find(val.first);
				if (curr != nullptr){
//					std::cout << val.first << " is already inserted silly" << std::endl;
					return std::make_pair(curr, false);
				}


				int levels = 1;
				while (std::rand()/(float)RAND_MAX < .5){
					levels++;
				}
				int path_size = std::min(height,levels);
				std::vector<Node *> path(path_size);
//				std::cout << "insert key: " << val.first;
//				std::cout << "\tlevels: " << levels;

				//build path for inserting node into existing skiplist
				curr = sentinel;
				//if height > levels
				for (int i = height; i > levels; i--){
					curr = curr->below;
				}
				Node *curr_sent = curr;
				int curr_level = path_size;
				while (curr != nullptr){
					while (curr->suc != curr_sent && static_cast<DataNode *>(curr->suc)->val.first < val.first){
						curr = curr->suc;
					}
					path[curr_level - 1] = curr;
					curr_level--;
					curr = curr->below;
					curr_sent = curr_sent->below;
				}

				//insert nodes into existing skiplist
				Node *inserted = sentinel;
				Node *bottom = sentinel;
				Node *build = sentinel ;
				for (auto i = path.begin(); i != path.end(); i++){
					build = new DataNode(val);
					build->pred = (*i);
					build->suc = (*i)->suc;
					(*i)->suc->pred = build;
					(*i)->suc = build;
					if (i != path.begin()){
						build->below = bottom;
						bottom->above = build;
					}
					else{
						inserted = build;
					}
					bottom = build;
				}

				//insert nodes into new levels
				bottom = inserted;
				//if height > 1
				while (bottom->above != nullptr){
					bottom = bottom->above;
				}
				Node *temp_sent;
				for (int i = height; i < levels; i++){
					build = new DataNode(val);
					temp_sent = new Node();
					build->pred = build->suc = temp_sent;
					build->below = bottom;
					bottom->above = build;
					bottom = build;
					temp_sent->pred = temp_sent->suc = build;
					temp_sent->below = sentinel;
					sentinel->above = temp_sent;
					sentinel = temp_sent;
				}
				if (height < levels){
					height = levels;
				}
				size++;
//				std::cout << "\tnew_height: " << height;
//				std::cout << "\tsize = " << size << std::endl;

				return std::make_pair(inserted, true);
			}

			void erase(const Key_T &key){
				Node *curr = find(key);
				if (curr == nullptr){
					return;
				}

				Node *bottom;
				while (curr != nullptr){
					bottom = curr;
					curr = curr->above;
					bottom->pred->suc = bottom->suc;
					bottom->suc->pred = bottom->pred;
					delete bottom;
				}
				size--;
			}
/*
			void print(){
				//print list
				std::cout << "list so far" << std::endl;
				Node *curr = sentinel;
				Node *curr_sent = sentinel;
				for (int i = height; i > 0; i--){
					std::cout << "[S] -> ";
					while (curr->suc != curr_sent){
						std::cout << "[" << static_cast<DataNode *>(curr->suc)->val.first << "] -> ";
						curr = curr->suc;
					}
					std::cout << "[S]" << std::endl;
					curr_sent = curr_sent->below;
					curr = curr_sent;
				}
			}
*/
			int getSize(){
				return size;
			}

			Node *getSentinel(){
				return sentinel;
			}

		private:
			int height;
			int size;
			Node *sentinel;
		};

	public:
		class Iterator{
		public:
			Iterator(typename SkipList::Node *n) : element(n) {}
			Iterator &operator++(){
				element = element->suc;
				return *this;
			}
			Iterator operator++(int){
				auto prev = *this;
				element = element->suc;
				return prev;
			}
			Iterator &operator--(){
				element = element->pred;
				return *this;
			}
			Iterator operator--(int){
				auto suc = *this;
				element = element->pred;
				return suc;
			}
			ValueType &operator*() const{
				return static_cast<typename SkipList::DataNode *>(element)->val;
			}
			ValueType *operator->() const{
				return &(static_cast<typename SkipList::DataNode *>(element)->val);
			}
			typename SkipList::Node *getElement() const{
				return element;
			}
		protected:
			typename SkipList::Node *element;
		};

		class ConstIterator : public Iterator{
		public:
			ConstIterator(typename SkipList::Node *n) : Iterator(n) {}
			ConstIterator(const Iterator &iter) : Iterator(iter){}
			const ValueType &operator*() const{
				return static_cast<typename SkipList::DataNode *>(this->getElement())->val;
			}
			const ValueType *operator->() const{
				return &(static_cast<typename SkipList::DataNode *>(this->getElement())->val);
			}
		};

		class ReverseIterator : public Iterator{
		public:
			ReverseIterator(typename SkipList::Node *n) : Iterator(n) {}
			ReverseIterator &operator++(){
				Iterator::operator--();
				return *this;
			}
			ReverseIterator operator++(int){
				auto prev = *this;
				Iterator::operator--();
				return prev;
			}
			ReverseIterator &operator--(){
				Iterator::operator++();
				return *this;
			}
			ReverseIterator operator--(int){
				auto prev = *this;
				Iterator::operator++();
				return prev;
			}
		};

		//Constructor: creates empty map
		Map() : skip_list(new SkipList()) {}

		//Copy Constructor
		Map(const Map &other) : skip_list(new SkipList()){
			for (auto i = other.begin(); i != other.end(); i++){
				skip_list->insert(*i);
			}
		}

		//Copy Assignment
		Map &operator=(Map other){
			/*
			if (*this == other){
				return *this;
			}
			delete skip_list;
			skip_list = new SkipList();
			for (auto i = other.begin(); i != other.end(); i++){
				skip_list->insert(*i);
			}
			*/
			swap(*this, other);
			return *this;
		}

		friend void swap(Map &m1, Map &m2){
			using std::swap;

			swap(m1.skip_list, m2.skip_list);
		}

		//Initializer List Constructor
		Map(std::initializer_list<std::pair<const Key_T, Mapped_T>> list) : skip_list(new SkipList()){
			for (auto e : list){
				skip_list->insert(e);
			}
		}

		~Map(){
			delete skip_list;
		}

		size_t size() const{
			return skip_list->getSize();
		}

		bool empty() const{
			return size() == 0;
		}

		Iterator begin(){
			return Iterator(bottomSentinel()->suc);
		}

		Iterator end(){
			return Iterator(bottomSentinel());
		}

		ConstIterator begin() const{
			return ConstIterator(bottomSentinel()->suc);
		}

		ConstIterator end() const{
			return ConstIterator(bottomSentinel());
		}

		ReverseIterator rbegin(){
			return ReverseIterator(bottomSentinel()->pred);
		}

		ReverseIterator rend(){
			return ReverseIterator(bottomSentinel());
		}

		Iterator find(const Key_T &key){
			auto n = skip_list->find(key);
			if (n == nullptr){
				return end();
			}
			return Iterator(n);
		}

		ConstIterator find(const Key_T &key) const{
			auto n = skip_list->find(key);
			if (n == nullptr){
				return end();
			}
			return ConstIterator(n);
		}

		Mapped_T &at(const Key_T &key){
			auto n = find(key);
			if (n == end()){
				throw std::out_of_range("key not in map\n");
			}
			return (*n).second;
		}

		const Mapped_T &at(const Key_T &key) const{
			auto n = find(key);
			if (n == end()){
				throw std::out_of_range("key not in map\n");
			}
			return (*n).second;
		}

		Mapped_T &operator[](const Key_T &key){
			auto n = find(key);
			if (n == end()){
				skip_list->insert(std::make_pair(key,Mapped_T()));
				return at(key);
			}
			return at(key);
		}

		std::pair<Iterator, bool> insert(const ValueType &val){
			auto n = skip_list->insert(val);
			return std::make_pair(Iterator(n.first),n.second);
		}

		template <typename IT_T>
		void insert(IT_T range_beg, IT_T range_end){
			for (auto i = range_beg; i != range_end; i++){
				skip_list->insert(*i);
			}
		}

		void erase(Iterator pos){
			skip_list->erase((*pos).first);
		}

		void erase(const Key_T &key){
			if (find(key)== end()){
				throw std::out_of_range("key not in map");
			}
			skip_list->erase(key);
		}

		void clear(){
			while (begin() != end()){
				erase(begin());
			}
		}

		friend bool operator==(const Map<Key_T, Mapped_T> &m1, const Map<Key_T, Mapped_T> &m2){
			if (m1.size() == m2.size()){
				auto i = m1.begin();
				auto j = m2.begin();
				while (i != m1.end()){
					if ((*i) != (*j)){
						return false;
					}
					i++;
					j++;
				}
				return true;
			}
			return false;
		}

		friend bool operator!=(const Map<Key_T, Mapped_T> &m1, const Map<Key_T, Mapped_T> &m2){
			return !(m1==m2);
		}

		friend bool operator<(const Map<Key_T, Mapped_T> &m1, const Map<Key_T, Mapped_T> &m2){
			auto i = m1.begin();
			auto j = m2.begin();
			while (i != m1.end()){
				if ((*i) < (*j)){
					return true;
				}
				else if ((*i) > (*j)){
					return false;
				}
				else{
					i++;
					j++;
				}
			}
			if (j != m2.end()){
				return true;
			}
			return false;
		}

		friend bool operator==(const Iterator &i1, const Iterator &i2){
			return i1.getElement() == i2.getElement();
		}

		friend bool operator!=(const Iterator &i1, const Iterator &i2){
			return !(i1==i2);
		}

	private:
		Map<Key_T, Mapped_T>::SkipList *skip_list;

		typename SkipList::Node *bottomSentinel() const{
			auto sentinel = skip_list->getSentinel();
			while (sentinel->below != nullptr){
				sentinel = sentinel->below;
			}
			return sentinel;
		}
	};

}
#endif
