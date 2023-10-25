#ifndef MERLIN_BASIC_PASSWORD_HPP
#define MERLIN_BASIC_PASSWORD_HPP

#include <iostream>
#include <algorithm>
#include <cstring>
#include <stdexcept>
#include <utility>
#include <limits>
#include <cstdint>

namespace merl
{
    template <typename CharT, typename Traits = std::char_traits<CharT>>
    class basic_password
    {
        public:
            using value_type = CharT;
            using size_type = std::size_t;
            using reference = value_type &;
            using const_reference = const value_type &; // reference to const
            using pointer = value_type *;
            using const_pointer = const value_type *;   // pointer to const
            using iterator = value_type *;
            using const_iterator = const value_type *;  // iterator to const
            class reverse_iterator;
            class const_reverse_iterator;

            static const size_type npos = -1;

        private:
            CharT * data_;
            size_type size_;

            void secure_del()
            {
                std::fill_n(data_, size_, 0);
                delete[] data_;
            }
            bool iterator_check(const_iterator cit) const
            {
                // bounds check
                if(cit < data_)
                    return false;
                if(cit > data_+size_)
                    return false;

                // Alignment check (unnecessary and useless if sizeof(CharT) == 1)
                return !((reinterpret_cast<uintptr_t>(cit) - reinterpret_cast<uintptr_t>(data_)) % sizeof(CharT));
            }

        public:
            // Constructors
            basic_password() : data_{new CharT[1]{}}, size_{0}
            {}
            basic_password(const CharT * p) : size_{0}
            {
                if(p)
                {
                    while(p[size_++]);

                    data_ = new CharT[size_];
                    Traits::copy(data_, p, size_--);
                }
                else
                    data_ = new CharT[size_+1]{};
            }
            basic_password(const CharT * p, size_type count) : size_{0}
            {
                if(p)
                {
                    size_ = count;
                    data_ = new CharT[size_+1]{};
                    Traits::copy(data_, p, size_);
                }
                else
                    data_ = new CharT[size_+1]{};
            }
            basic_password(size_type count, CharT c) : size_{count}
            {
                data_ = new CharT[size_+1]{};
                std::fill_n(data_, size_, c);
            }
            template <typename InputIt>
            basic_password(InputIt first, InputIt last) : size_{static_cast<size_type>(std::distance(first, last))}
            {
                data_ = new CharT[size_+1]{};
                std::copy(first, last, data_);
            }
            basic_password(std::initializer_list<CharT> il) : basic_password(il.begin(), il.end())
            {}

            basic_password(const basic_password & other) : size_{other.size_}
            {
                data_ = new CharT[size_+1]{};
                Traits::copy(data_, other.data_, size_);
            }
            basic_password(basic_password && other) noexcept : data_{std::exchange(other.data_, new CharT[1]{})}, size_{std::exchange(other.size_, 0)}
            {}

            // Destructor
            ~basic_password()
            {
                secure_del();
            }

            // Assignment
            basic_password & operator=(const basic_password & other)
            {
                if(&other != this)
                {
                    secure_del();

                    size_ = other.size_;
                    data_ = new CharT[size_+1]{};
                    Traits::copy(data_, other.data_, size_);
                }
                return *this;
            }
            basic_password & operator=(basic_password && other) noexcept
            {
                if(&other != this)
                {
                    size_ = std::exchange(other.size_, 0);
                    data_ = std::exchange(other.data_, new CharT[1]{});
                }
                return *this;
            }

            basic_password & operator=(const CharT * p)
            {
                secure_del();

                size_ = 0;
                while(p[size_++]);

                data_ = new CharT[size_];
                Traits::copy(data_, p, size_--);

                return *this;
            }
            basic_password & operator=(std::nullptr_t) = delete;
            basic_password & operator=(CharT c)
            {
                secure_del();

                size_ = 1;
                data_ = new CharT[size_+1]{};
                data_[0] = c;

                return *this;
            }
            basic_password & operator=(std::initializer_list<CharT> il)
            {
                secure_del();

                size_ = il.size();
                data_ = new CharT[size_+1]{};
                std::copy(il.begin(), il.end(), data_);

                return *this;
            }

            // Elements access
            const_reference operator[](size_type pos) const
            {
                return data_[pos];
            }
            reference operator[](size_type pos)
            {
                return data_[pos];
            }
            const_reference & at(size_type pos) const
            {
                if(pos >= size_)
                    throw std::out_of_range("merl::basic_password::at(): Out of range (pos = " + std::to_string(pos) + ", size = " + std::to_string(size_) + ')');
                
                return data_[pos];
            }
            reference at(size_type pos)
            {
                return const_cast<CharT &>(const_cast<const basic_password<CharT> &>(*this).at(pos));
            }

            const_pointer data() const noexcept
            {
                return data_;
            }
            pointer data() noexcept
            {
                return data_;
            }

            const_reference front() const
            {
                return data_[0];
            }
            reference front()
            {
                return data_[0];
            }
            const_reference back() const
            {
                return data_[size_-1];
            }
            reference back()
            {
                return data_[size_-1];
            }

            // Capacity
            bool empty() const
            {
                return !size_;
            }
            size_type size() const
            {
                return size_;
            }
            size_type max_size() const noexcept
            {
                return std::numeric_limits<size_type>::max()-1;
            }

            // Iterators
            const_iterator cbegin() const
            {
                return data_;
            }
            const_iterator begin() const
            {
                return cbegin();
            }
            iterator begin()
            {
                return data_;
            }
            const_iterator cend() const
            {
                return data_+size_;
            }
            const_iterator end() const
            {
                return cend();
            }
            iterator end()
            {
                return data_+size_;
            }
            const_reverse_iterator crbegin() const
            {
                const_reverse_iterator crit;
                crit.it_ = data_+size_;
                return crit;
            }
            reverse_iterator rbegin()
            {
                reverse_iterator rit;
                rit.it_ = data_+size_;
                return rit;
            }
            const_reverse_iterator crend() const
            {
                const_reverse_iterator crit;
                crit.it_ = data_;
                return crit;
            }
            reverse_iterator rend()
            {
                reverse_iterator rit;
                rit.it_ = data_;
                return rit;
            }

            // Operations
            void clear()
            {
                secure_del();
                size_ = 0;
                data_ = new CharT[size_+1]{};
            }
            basic_password & insert(size_type index, size_type count, CharT c)
            {
                if(index > size_)
                    throw std::out_of_range("merl::basic_password::insert(): Out of range (index = " + std::to_string(index) + ", size = " + std::to_string(size_) + ')');
                if(size_ > max_size() - count)
                    throw std::length_error("merl::basic_password::insert(): Length error -> Maximum size exceeded");

                if(!count)
                    return *this;

                size_type target_size = size_ + count;
                CharT * target = new CharT[target_size + 1]{};

                Traits::copy(target, data_, index);
                std::fill_n(target + index, count, c);
                Traits::copy(target + index+count, data_ + index, size_ - index);

                secure_del();
                data_ = target;
                size_ = target_size;

                return *this;
            }
            basic_password & insert(size_type index, const CharT * p)
            {
                size_type length = Traits::length(p);

                if(index > size_)
                    throw std::out_of_range("merl::basic_password::insert(): Out of range (index = " + std::to_string(index) + ", size = " + std::to_string(size_) + ')');
                if(size_ > max_size() - length)
                    throw std::length_error("merl::basic_password::insert(): Length error -> Maximum size exceeded");

                if(!length)
                    return *this;

                size_type target_size = size_ + length;
                CharT * target = new CharT[target_size + 1]{};

                Traits::copy(target, data_, index);
                Traits::copy(target + index, p, length);
                Traits::copy(target + index+length, data_ + index, size_ - index);

                secure_del();
                data_ = target;
                size_ = target_size;

                return *this;
            }
            basic_password & insert(size_type index, const CharT * p, size_type count)
            {
                if(index > size_)
                    throw std::out_of_range("merl::basic_password::insert(): Out of range (index = " + std::to_string(index) + ", size = " + std::to_string(size_) + ')');
                if(size_ > max_size() - count)
                    throw std::length_error("merl::basic_password::insert(): Length error -> Maximum size exceeded");

                if(!count)
                    return *this;

                size_type target_size = size_ + count;
                CharT * target = new CharT[target_size + 1]{};

                Traits::copy(target, data_, index);
                Traits::copy(target + index, p, count);
                Traits::copy(target + index+count, data_ + index, size_ - index);

                secure_del();
                data_ = target;
                size_ = target_size;

                return *this;
            }
            basic_password & insert(size_type index, const basic_password & p)
            {
                if(index > size_)
                    throw std::out_of_range("merl::basic_password::insert(): Out of range (index = " + std::to_string(index) + ", size = " + std::to_string(size_) + ')');
                if(size_ > max_size() - p.size())
                    throw std::length_error("merl::basic_password::insert(): Length error -> Maximum size exceeded");

                if(!p.size_)
                    return *this;

                size_type target_size = size_ + p.size_;
                CharT * target = new CharT[target_size + 1]{};

                Traits::copy(target, data_, index);
                Traits::copy(target + index, p.data_, p.size_);
                Traits::copy(target + index+p.size_, data_ + index, size_ - index);

                secure_del();
                data_ = target;
                size_ = target_size;

                return *this;
            }
            basic_password & insert(size_type index, const basic_password & p, size_type p_index, size_type count = npos)
            {
                if(index > size_)
                    throw std::out_of_range("merl::basic_password::insert(): Out of range (index = " + std::to_string(index) + ", size = " + std::to_string(size_) + ')');
                if(p_index > p.size_)
                    throw std::out_of_range("merl::basic_password::insert(): Out of range (index = " + std::to_string(p_index) + ", size = " + std::to_string(p.size_) + ')');
                if(count > p.size_ - p_index)
                    count = p.size_ - p_index;
                if(size_ > max_size() - count)
                    throw std::length_error("merl::basic_password::insert(): Length error -> Maximum size exceeded");

                if(!count)
                    return *this;

                size_type target_size = size_ + count;
                CharT * target = new CharT[target_size + 1]{};

                Traits::copy(target, data_, index);
                Traits::copy(target + index, p.data_ + p_index, count);
                Traits::copy(target + index+count, data_ + index, size_ - index);

                secure_del();
                data_ = target;
                size_ = target_size;

                return *this;
            }
            iterator insert(const_iterator pos, CharT c)
            {
                if(!iterator_check(pos))
                    throw std::invalid_argument("merl::basic_password::insert(): Invalid argument -> Invalid iterator");
                if(size_ == max_size())
                    throw std::length_error("merl::basic_password::insert(): Length error -> Maximum size exceeded");

                size_type index = pos - data_;
                insert(index, 1, c);
                return data_ + index;
            }
            iterator insert(const_iterator pos, size_type count, CharT c)
            {
                if(!iterator_check(pos))
                    throw std::invalid_argument("merl::basic_password::insert(): Invalid argument -> Invalid iterator");
                if(size_ > max_size() - count)
                    throw std::length_error("merl::basic_password::insert(): Length error -> Maximum size exceeded");

                size_type index = pos - data_;
                insert(index, count, c);
                return data_ + index;
            }
            template <typename InputIt>
            iterator insert(const_iterator pos, InputIt first, InputIt last)
            {
                if(!iterator_check(pos))
                    throw std::invalid_argument("merl::basic_password::insert(): Invalid argument -> Invalid iterator");
                size_type length = std::distance(first, last);
                if(size_ > max_size() - length)
                    throw std::length_error("merl::basic_password::insert(): Length error -> Maximum size exceeded");

                if(!length)
                    return const_cast<iterator>(pos); // Legal because *this is non-const
                
                size_type index = pos - data_;
                size_type target_size = size_ + length;
                CharT * target = new CharT[target_size + 1]{};

                Traits::copy(target, data_, index);
                std::copy(first, last, target + index);
                Traits::copy(target + index+length, data_ + index, size_ - index);

                secure_del();
                data_ = target;
                size_ = target_size;

                return data_ + index;
            }
            iterator insert(const_iterator pos, std::initializer_list<CharT> ilist)
            {
                return insert(pos, ilist.begin(), ilist.end());
            }

            basic_password & erase(size_type index = 0, size_type count = npos)
            {
                if(index > size_)
                    throw std::out_of_range("merl::basic_password::erase(): Out of range (index = " + std::to_string(index) + ", size = " + std::to_string(size_) + ')');
                if(count > size_ - index)
                    count = size_ - index;

                if(!count)
                    return *this;

                size_type target_size = size_ - count;
                CharT * target = new CharT[target_size + 1]{};

                Traits::copy(target, data_, index);
                Traits::copy(target + index, data_ + index+count, size_ - (index+count));

                secure_del();
                data_ = target;
                size_ = target_size;

                return *this;
            }
            iterator erase(const_iterator position)
            {
                if(!iterator_check(position))
                    throw std::invalid_argument("merl::basic_password::erase(): Invalid argument -> Invalid iterator");

                size_type index = position - data_;
                erase(index, 1);
                return data_ + index;
            }
            iterator erase(const_iterator first, const_iterator last)
            {
                if(! (iterator_check(first) && iterator_check(last)) )
                    throw std::invalid_argument("merl::basic_password::erase(): Invalid argument -> Invalid iterator");

                size_type index = first - data_;
                erase(index, std::distance(first, last));
                return data_ + index;
            }

            void push_back(CharT c)
            {
                insert(size_, 1, c);
            }
            void pop_back()
            {
                erase(size_-1, 1);
            }

            basic_password & append(size_type count, CharT c)
            {
                return insert(size_, count, c);
            }
            basic_password & append(const basic_password & p)
            {
                return insert(size_, p);
            }
            basic_password & append(const basic_password & p, size_type pos, size_type count = npos)
            {
                return insert(size_, p, pos, count);
            }
            basic_password & append(const CharT * p, size_type count)
            {
                return insert(size_, p, count);
            }
            basic_password & append(const CharT * p)
            {
                return insert(size_, p);
            }
            template <typename InputIt>
            basic_password & append(InputIt first, InputIt last)
            {
                insert(data_ + size_, first, last);
                return *this;
            }
            basic_password & append(std::initializer_list<CharT> ilist)
            {
                insert(data_ + size_, ilist);
                return *this;
            }

            basic_password & operator+=(const basic_password & p)
            {
                return append(p);
            }
            basic_password & operator+=(CharT c)
            {
                return append(1, c);
            }
            basic_password & operator+=(const CharT * p)
            {
                return append(p);
            }
            basic_password & operator+=(std::initializer_list<CharT> ilist)
            {
                return append(ilist);
            }

            int compare(const basic_password & p) const
            {
                int result = Traits::compare(data_, p.data_, std::min(size_, p.size_));

                if(!result)
                {
                    if(size_ < p.size_)
                        result = -1;
                    else if(size_ > p.size_)
                        result = 1;
                }

                return result;
            }
            int compare(size_type pos1, size_type count1, const basic_password & p) const
            {
                if(pos1 >= size_)
                    throw std::out_of_range("merl::basic_password::compare(): Out of range (position = " + std::to_string(pos1) + ", size = " + std::to_string(size_) + ')');

                if(count1 > size_ - pos1)
                    count1 = size_ - pos1;

                int result = Traits::compare(data_ + pos1, p.data_, std::min(count1, p.size_));

                if(!result)
                {
                    if(count1 < p.size_)
                        return -1;
                    else if(count1 > p.size_)
                        return 1;
                }

                return result;
            }
            int compare(size_type pos1, size_type count1, const basic_password & p, size_type pos2, size_type count2 = npos) const
            {
                if(pos1 >= size_)
                    throw std::out_of_range("merl::basic_password::compare(): Out of range (position = " + std::to_string(pos1) + ", size = " + std::to_string(size_) + ')');
                if(pos2 >= p.size_)
                    throw std::out_of_range("merl::basic_password::compare(): Out of range (position = " + std::to_string(pos2) + ", size = " + std::to_string(p.size_) + ')');

                if(count1 > size_ - pos1)
                    count1 = size_ - pos1;
                if(count2 > p.size_ - pos2)
                    count2 = p.size_ - pos2;

                int result = Traits::compare(data_ + pos1, p.data_ + pos2, std::min(count1, count2));

                if(!result)
                {
                    if(count1 < count2)
                        return -1;
                    else if(count1 > count2)
                        return 1;
                }

                return result;
            }
            int compare(const CharT * p) const
            {
                std::size_t length = Traits::length(p);
                int result = Traits::compare(data_, p, std::min(size_, length));

                if(!result)
                {
                    if(size_ < length)
                        result = -1;
                    else if(size_ > length)
                        result = 1;
                }

                return result;
            }
            int compare(size_type pos1, size_type count1, const CharT * p) const
            {
                if(pos1 >= size_)
                    throw std::out_of_range("merl::basic_password::compare(): Out of range (position = " + std::to_string(pos1) + ", size = " + std::to_string(size_) + ')');

                if(count1 > size_ - pos1)
                    count1 = size_ - pos1;

                std::size_t length = Traits::length(p);
                int result = Traits::compare(data_ + pos1, p, std::min(count1, length));

                if(!result)
                {
                    if(count1 < length)
                        return -1;
                    else if(count1 > length)
                        return 1;
                }

                return result;
            }
            int compare(size_type pos1, size_type count1, const CharT * p, size_type count2) const
            {
                if(pos1 >= size_)
                    throw std::out_of_range("merl::basic_password::compare(): Out of range (position = " + std::to_string(pos1) + ", size = " + std::to_string(size_) + ')');
                
                if(count1 > size_ - pos1)
                    count1 = size_ - pos1;

                int result = Traits::compare(data_ + pos1, p, std::min(count1, count2));

                if(!result)
                {
                    if(count1 < count2)
                        return -1;
                    else if(count1 > count2)
                        return 1;
                }

                return result;
            }

            bool starts_with(CharT c) const noexcept
            {
                return (size_ && Traits::eq(data_[0], c));
            }
            bool starts_with(const CharT * p) const
            {
                std::size_t length = Traits::length(p);

                if(size_ < length)
                    return false;

                for(std::size_t i = 0; i < length; ++i)
                {
                    if(!Traits::eq(data_[i], p[i]))
                        return false;
                }
                
                return true;
            }
            bool ends_with(CharT c) const noexcept
            {
                return(size_ && Traits::eq(data_[size_ - 1], c));
            }
            bool ends_with(const CharT * p) const
            {
                std::size_t length = Traits::length(p);

                if(size_ < length)
                    return false;

                for(std::size_t i = 0; i < length; ++i)
                {
                    if(!Traits::eq(data_[size_-1 - i], p[length-1 - i]))
                        return false;
                }

                return true;
            }
            
            bool contains(CharT c) const noexcept
            {
                for(std::size_t i = 0; i < size_; ++i)
                {
                    if(Traits::eq(data_[i], c))
                        return true;
                }
                return false;
            }
            bool contains(const CharT * p) const
            {
                std::size_t length = Traits::length(p);
                if(!length)
                    return true;

                std::size_t pidx {};
                for(std::size_t i = 0; i < size_; ++i)
                {
                    if(Traits::eq(data_[i], p[pidx]))
                    {
                        if(++pidx == length)
                            return true;
                    }
                    else if(Traits::eq(data_[i], p[0]))
                        pidx = 1;
                    else
                        pidx = 0;
                }
                return false;
            }

            basic_password & replace(size_type pos, size_type count, const basic_password & p)
            {
                if(pos >= size_)
                    throw std::out_of_range("merl::basic_password::replace(): Out of range (position = " + std::to_string(pos) + ", size = " + std::to_string(size_) + ')');
                size_type nb_to_rm = std::min(count, size_ - pos);
                if(size_ - nb_to_rm > max_size() - p.size_)
                    throw std::length_error("merl::basic_password::replace(): Length error -> Maximum size exceeded");
                
                if(!nb_to_rm)
                    return insert(pos, p);

                if(nb_to_rm == p.size_)
                {
                    Traits::copy(data_ + pos, p.data_, p.size_);
                }
                else
                {
                    size_type target_size = size_ - nb_to_rm + p.size_;
                    CharT * target = new CharT[target_size + 1]{};

                    Traits::copy(target, data_, pos);
                    Traits::copy(target + pos, p.data_, p.size_);
                    Traits::copy(target + pos+p.size_, data_ + pos+nb_to_rm, size_ - (pos + nb_to_rm));

                    secure_del();
                    data_ = target;
                    size_ = target_size;
                }

                return *this;
            }
            basic_password & replace(const_iterator first, const_iterator last, const basic_password & p)
            {
                if(! (iterator_check(first) && iterator_check(last)) )
                    throw std::invalid_argument("merl::basic_password::replace(): Invalid argument -> Invalid iterator");
                
                return replace(first - data_, std::distance(first, last), p);
            }
            basic_password & replace(size_type pos, size_type count, const basic_password & p, size_type pos2, size_type count2 = npos)
            {
                if(pos >= size_)
                    throw std::out_of_range("merl::basic_password::replace(): Out of range (position = " + std::to_string(pos) + ", size = " + std::to_string(size_) + ')');
                if(pos2 >= p.size_)
                    throw std::out_of_range("merl::basic_password::replace(): Out of range (position = " + std::to_string(pos2) + ", size = " + std::to_string(p.size_) + ')');
                size_type nb_to_rm = std::min(count, size_ - pos);
                size_type nb_to_place = std::min(count2, p.size_ - pos2);
                if(size_ - nb_to_rm > max_size() - nb_to_place)
                    throw std::length_error("merl::basic_password::replace(): Length error -> Maximum size exceeded");

                if(!nb_to_rm)
                    return insert(pos, p, pos2, nb_to_place);
                if(!nb_to_place)
                    return erase(pos, nb_to_rm);

                if(nb_to_rm == nb_to_place)
                {
                    Traits::copy(data_ + pos, p.data_ + pos2, nb_to_place);
                }
                else
                {
                    size_type target_size = size_ - nb_to_rm + nb_to_place;
                    CharT * target = new CharT[target_size + 1]{};

                    Traits::copy(target, data_, pos);
                    Traits::copy(target + pos, p.data_ + pos2, nb_to_place);
                    Traits::copy(target + pos+nb_to_place, data_ + pos+nb_to_rm, size_ - (pos + nb_to_rm));

                    secure_del();
                    data_ = target;
                    size_ = target_size;
                }

                return *this;
            }
            basic_password & replace(size_type pos, size_type count, const CharT * p, size_type count2)
            {
                if(pos >= size_)
                    throw std::out_of_range("merl::basic_password::replace(): Out of range (position = " + std::to_string(pos) + ", size = " + std::to_string(size_) + ')');
                size_type nb_to_rm = std::min(count, size_ - pos);
                if(size_ - nb_to_rm > max_size() - count2)
                    throw std::length_error("merl::basic_password::replace(): Length error -> Maximum size exceeded");

                if(!nb_to_rm)
                    return insert(pos, p, count2);
                if(!count2)
                    return erase(pos, nb_to_rm);

                if(nb_to_rm == count2)
                {
                    Traits::copy(data_ + pos, p, count2);
                }
                else
                {
                    size_type target_size = size_ - nb_to_rm + count2;
                    CharT * target = new CharT[target_size + 1]{};

                    Traits::copy(target, data_, pos);
                    Traits::copy(target + pos, p, count2);
                    Traits::copy(target + pos+count2, data_ + pos+nb_to_rm, size_ - (pos + nb_to_rm));

                    secure_del();
                    data_ = target;
                    size_ = target_size;
                }

                return *this;
            }
            basic_password & replace(const_iterator first, const_iterator last, const CharT * p, size_type count2)
            {
                if(! (iterator_check(first) && iterator_check(last)) )
                    throw std::invalid_argument("merl::basic_password::replace(): Invalid argument -> Invalid iterator");

                return replace(first - data_, std::distance(first, last), p, count2);
            }
            basic_password & replace(size_type pos, size_type count, const CharT * p)
            {
                return replace(pos, count, p, Traits::length(p));
            }
            basic_password & replace(const_iterator first, const_iterator last, const CharT * p)
            {
                if(! (iterator_check(first) && iterator_check(last)) )
                    throw std::invalid_argument("merl::basic_password::replace(): Invalid argument -> Invalid iterator");

                return replace(first - data_, std::distance(first, last), p);
            }
            basic_password & replace(size_type pos, size_type count, size_type count2, CharT c)
            {
                if(pos >= size_)
                    throw std::out_of_range("merl::basic_password::replace(): Out of range (position = " + std::to_string(pos) + ", size = " + std::to_string(size_) + ')');
                size_type nb_to_rm = std::min(count, size_ - pos);
                if(size_ - nb_to_rm > max_size() - count2)
                    throw std::length_error("merl::basic_password::replace(): Length error -> Maximum size exceeded");
                
                if(!nb_to_rm)
                    return insert(pos, count2, c);
                if(!count2)
                    return erase(pos, nb_to_rm);

                if(nb_to_rm == count2)
                {
                    std::fill_n(data_ + pos, count2, c);
                }
                else
                {
                    size_type target_size = size_ - nb_to_rm + count2;
                    CharT * target = new CharT[target_size + 1]{};

                    Traits::copy(target, data_, pos);
                    std::fill_n(target + pos, count2, c);
                    Traits::copy(target + pos+count2, data_ + pos+nb_to_rm, size_ - (pos + nb_to_rm));

                    secure_del();
                    data_ = target;
                    size_ = target_size;
                }

                return *this;
            }
            basic_password & replace(const_iterator first, const_iterator last, size_type count2, CharT c)
            {
                if(! (iterator_check(first) && iterator_check(last)) )
                    throw std::invalid_argument("merl::basic_password::replace(): Invalid argument -> Invalid iterator");

                return replace(first - data_, std::distance(first, last), count2, c);
            }
            template <typename InputIt>
            basic_password & replace(const_iterator first, const_iterator last, InputIt first2, InputIt last2)
            {
                if(! (iterator_check(first) && iterator_check(last)) )
                    throw std::invalid_argument("merl::basic_password::replace(): Invalid argument -> Invalid iterator");
                size_type count = std::distance(first, last);
                size_type count2 = std::distance(first2, last2);
                if(size_ - count > max_size() - count2)
                    throw std::length_error("merl::basic_password::replace(): Length error -> Maximum size exceeded");
                
                if(!count)
                {
                    insert(first, first2, last2);
                }
                else if(!count2)
                {
                    erase(first, last);
                }
                else if(count == count2)
                {
                    std::copy(first2, last2, const_cast<iterator>(first)); // Legal because *this is non-const
                }
                else
                {
                    size_type index = first - data_;
                    size_type target_size = size_ - count + count2;
                    CharT * target = new CharT[target_size + 1]{};

                    Traits::copy(target, data_, index);
                    std::copy(first2, last2, target + (first - data_));
                    Traits::copy(target + index+count2, data_ + index+count, size_ - (index + count));

                    secure_del();
                    data_ = target;
                    size_ = target_size;
                }

                return *this;
            }
            basic_password & replace(const_iterator first, const_iterator last, std::initializer_list<CharT> ilist)
            {
                return replace(first, last, ilist.begin(), ilist.end());
            }

            basic_password subpwd(size_type pos = 0, size_type count = npos) const
            {
                if(pos > size_)
                    throw std::out_of_range("merl::basic_password::subpwd(): Out of range (position = " + std::to_string(pos) + ", size = " + std::to_string(size_) + ')');
                
                return basic_password(data_ + pos, data_ + pos + std::min(count, size_ - pos)); // If pos == size_, will call equivalent to basic_password(end(), end()), which will be empty basic_password.
            }

            size_type copy(CharT * dest, size_type count, size_type pos = 0) const
            {
                if(pos > size_)
                    throw std::out_of_range("merl::basic_password::copy(): Out of range (position = " + std::to_string(pos) + ", size = " + std::to_string(size_) + ')');
                
                if(count > size_ - pos)
                    count = size_ - pos;

                Traits::copy(dest, data_ + pos, count); // If pos == size_, will call equivalent to Traits::copy(dest, end(), 0), which will copy nothing.
                return count;
            }

            void resize(size_type count, CharT c = CharT{})
            {
                if(count > max_size())
                    throw std::length_error("merl::basic_password::resize(): Length error -> Maximum size exceeded");

                if(count != size_)
                {
                    CharT * target = new CharT[count + 1]{};

                    if(count < size_)
                    {
                        Traits::copy(target, data_, count);
                    }
                    else
                    {
                        Traits::copy(target, data_, size_);
                        std::fill_n(target + size_, count - size_, c);
                    }

                    secure_del();
                    data_ = target;
                    size_ = count;
                }
            }

            void swap(basic_password & other) noexcept
            {
                using std::swap;
                swap(data_, other.data_);
                swap(size_, other.size_);
            }

            // Search
            size_type find(const basic_password & p, size_type pos = 0) const noexcept
            {
                return find(p.data(), pos, p.size_);
            }
            size_type find(const CharT * p, size_type pos, size_type count) const
            {
                if(pos >= size_)
                    return npos;
                if(!count)
                    return pos;

                size_type pidx {};
                for(size_type i = pos; i < size_; ++i)
                {
                    if(Traits::eq(data_[i], p[pidx]))
                    {
                        if(++pidx == count)
                            return i+1-count;
                    }
                    else if(Traits::eq(data_[i], p[0]))
                        pidx = 1;
                    else
                        pidx = 0;
                }

                return npos;
            }
            size_type find(const CharT * p, size_type pos = 0) const
            {
                return find(p, pos, Traits::length(p));
            }
            size_type find(CharT c, size_type pos = 0) const noexcept
            {
                if(pos >= size_)
                    return npos;

                for(size_type i = pos; i < size_; ++i)
                {
                    if(Traits::eq(data_[i], c))
                        return i;
                }
                return npos;
            }

            size_type rfind(const basic_password & p, size_type pos = npos) const noexcept
            {
                return rfind(p.data(), pos, p.size_);
            }
            size_type rfind(const CharT * p, size_type pos, size_type count) const
            {
                if(!size_)
                    return npos;
                if(!count)
                    return (pos < size_ ? pos : size_);
                if(pos >= size_)
                    pos = size_-1;

                size_type pidx{};
                for(size_type i = 0; i <= pos; ++i)
                {
                    if(Traits::eq(data_[pos - i], p[count-1 - pidx]))
                    {
                        if(++pidx == count)
                            return pos-i;
                    }
                    else if(Traits::eq(data_[pos - i], p[count-1]))
                        pidx = 1;
                    else
                        pidx = 0;
                }

                return npos;
            }
            size_type rfind(const CharT * p, size_type pos = npos) const
            {
                return rfind(p, pos, Traits::length(p));
            }
            size_type rfind(CharT c, size_type pos = npos) const noexcept
            {
                if(!size_)
                    return npos;
                if(pos >= size_)
                    pos = size_-1;

                for(size_type i = 0; i <= pos; ++i)
                {
                    if(Traits::eq(data_[pos - i], c))
                        return pos - i;
                }
                return npos;
            }

            // TODO...

        // --- Reverse iterator definition (CRTP) ---
        private:
            template <typename Actual>
            class reverse_iterator_impl
            {
                friend class basic_password;

                protected:
                    iterator it_;

                public:
                    reverse_iterator_impl() : it_{nullptr}
                    {}
                    reverse_iterator_impl(iterator it) : it_{it}
                    {}
                    reverse_iterator_impl(const reverse_iterator_impl & other) : it_{other.it_}
                    {}
                    Actual & operator=(const Actual & other)
                    {
                        it_ = other.it_;
                        return static_cast<Actual &>(*this);
                    }
                    Actual & operator++() // prefix
                    {
                        if(it_)
                            --it_;
                        return static_cast<Actual &>(*this);
                    }
                    Actual operator++(int) // postfix
                    {
                        Actual tmp(static_cast<const Actual &>(*this));
                        ++(*this);
                        return tmp;
                    }
                    Actual & operator+=(size_type offset)
                    {
                        it_ -= offset;
                        return static_cast<Actual &>(*this);
                    }
                    Actual & operator--() // prefix
                    {
                        if(it_)
                            ++it_;
                        return static_cast<Actual &>(*this);
                    }
                    Actual operator--(int) // postfix
                    {
                        Actual tmp(static_cast<const Actual &>(*this));
                        --(*this);
                        return tmp;
                    }
                    Actual & operator-=(size_type offset)
                    {
                        it_ += offset;
                        return static_cast<Actual &>(*this);
                    }

                    friend bool operator==(const Actual & lhs, const Actual & rhs)
                    {
                        return lhs.it_ == rhs.it_;
                    }
                    friend bool operator!=(const Actual & lhs, const Actual & rhs)
                    {
                        return !(lhs == rhs);
                    }
                    friend Actual operator+(Actual lhs, size_type rhs)
                    {
                        return lhs += rhs;
                    }
                    friend Actual operator+(size_type lhs, Actual rhs)
                    {
                        return rhs += lhs;
                    }
                    friend Actual operator-(Actual lhs, size_type rhs)
                    {
                        return lhs -= rhs;
                    }
                    friend Actual operator-(size_type lhs, Actual rhs)
                    {
                        return rhs -= lhs;
                    }
            };

        public:
            class const_reverse_iterator : public reverse_iterator_impl<const_reverse_iterator>
            {
                public:
                    using reverse_iterator_impl<const_reverse_iterator>::reverse_iterator_impl; // inherit constructors

                    const value_type & operator*() const
                    {
                        iterator tmp = this->it_;
                        return *(--tmp);
                    }
                    const value_type * operator->() const
                    {
                        iterator tmp = this->it_;
                        return --tmp;
                    }
            };
            class reverse_iterator : public reverse_iterator_impl<reverse_iterator>
            {
                public:
                    using reverse_iterator_impl<reverse_iterator>::reverse_iterator_impl; // inherit constructors

                    value_type & operator*()
                    {
                        iterator tmp = this->it_;
                        return *(--tmp);
                    }
                    value_type * operator->()
                    {
                        iterator tmp = this->it_;
                        return --tmp;
                    }
            };
    };

    // Non-member functions
    template <typename CharT, typename Traits>
    std::basic_ostream<CharT, Traits> & operator<<(std::basic_ostream<CharT, Traits> & os, const basic_password<CharT, Traits> & p)
    {
        return (os << p.data());
    }
    template <typename CharT, typename Traits>
    std::basic_istream<CharT, Traits> & operator>>(std::basic_istream<CharT, Traits> & is, basic_password<CharT, Traits> & p)
    {
        static const std::size_t SIZE = 1024;
        CharT buffer[SIZE];
        while(is.read(buffer, SIZE))
            p.append(buffer, SIZE);
        p.append(buffer, is.gcount());
        return is;
    }

    template <typename CharT, typename Traits>
    bool operator==(const basic_password<CharT, Traits> & lhs, const basic_password<CharT, Traits> & rhs)
    {
        return !lhs.compare(rhs);
    }
    template <typename CharT, typename Traits>
    bool operator==(const basic_password<CharT, Traits> & lhs, const CharT * rhs)
    {
        return !lhs.compare(rhs);
    }
    template <typename CharT, typename Traits>
    Traits::comparison_category operator<=>(const basic_password<CharT, Traits> & lhs, const basic_password<CharT, Traits> & rhs)
    {
        if(lhs.compare(rhs) < 0)
            return Traits::comparison_category::less;

        if(lhs.compare(rhs) > 0)
            return Traits::comparison_category::greater;

        return Traits::comparison_category::equal;
    }
    template <typename CharT, typename Traits>
    Traits::comparison_category operator<=>(const basic_password<CharT, Traits> & lhs, const CharT * rhs)
    {
        if(lhs.compare(rhs) < 0)
            return Traits::comparison_category::less;

        if(lhs.compare(rhs) > 0)
            return Traits::comparison_category::greater;

        return Traits::comparison_category::equal;
    }

    template <typename CharT, typename Traits>
    void swap(basic_password<CharT, Traits> & lhs, basic_password<CharT, Traits> & rhs) noexcept
    {
        lhs.swap(rhs);
    }

    template <typename CharT, typename Traits>
    basic_password<CharT, Traits> operator+(const basic_password<CharT, Traits> & lhs, const basic_password<CharT, Traits> & rhs)
    {
        return basic_password<CharT, Traits>(lhs) += rhs;
    }
    template <typename CharT, typename Traits>
    basic_password<CharT, Traits> operator+(const basic_password<CharT, Traits> & lhs, const CharT * rhs)
    {
        return basic_password<CharT, Traits>(lhs) += rhs;
    }
    template <typename CharT, typename Traits>
    basic_password<CharT, Traits> operator+(const basic_password<CharT, Traits> & lhs, CharT rhs)
    {
        return basic_password<CharT, Traits>(lhs) += rhs;
    }
    template <typename CharT, typename Traits>
    basic_password<CharT, Traits> operator+(const CharT * lhs, const basic_password<CharT, Traits> & rhs)
    {
        return basic_password<CharT, Traits>(lhs) += rhs;
    }
    template <typename CharT, typename Traits>
    basic_password<CharT, Traits> operator+(CharT lhs, const basic_password<CharT, Traits> & rhs)
    {
        return basic_password<CharT, Traits>(1, lhs) += rhs;
    }
    template <typename CharT, typename Traits>
    basic_password<CharT, Traits> operator+(basic_password<CharT, Traits> && lhs, basic_password<CharT, Traits> && rhs)
    {
        return basic_password<CharT, Traits>(std::move(lhs)) += rhs;
    }
    template <typename CharT, typename Traits>
    basic_password<CharT, Traits> operator+(basic_password<CharT, Traits> && lhs, const basic_password<CharT, Traits> & rhs)
    {
        return basic_password<CharT, Traits>(std::move(lhs)) += rhs;
    }
    template <typename CharT, typename Traits>
    basic_password<CharT, Traits> operator+(basic_password<CharT, Traits> && lhs, const CharT * rhs)
    {
        return basic_password<CharT, Traits>(std::move(lhs)) += rhs;
    }
    template <typename CharT, typename Traits>
    basic_password<CharT, Traits> operator+(basic_password<CharT, Traits> && lhs, CharT rhs)
    {
        return basic_password<CharT, Traits>(std::move(lhs)) += rhs;
    }
    template <typename CharT, typename Traits>
    basic_password<CharT, Traits> operator+(const basic_password<CharT, Traits> & lhs, basic_password<CharT, Traits> && rhs)
    {
        return basic_password<CharT, Traits>(std::move(rhs)).insert(0, lhs);
    }
    template <typename CharT, typename Traits>
    basic_password<CharT, Traits> operator+(const CharT * lhs, basic_password<CharT, Traits> && rhs)
    {
        return basic_password<CharT, Traits>(std::move(rhs)).insert(0, lhs);
    }
    template <typename CharT, typename Traits>
    basic_password<CharT, Traits> operator+(CharT lhs, basic_password<CharT, Traits> && rhs)
    {
        return basic_password<CharT, Traits>(std::move(rhs)).insert(static_cast<basic_password<CharT, Traits>::size_type>(0), 1, lhs);
    }

    template <typename CharT, typename Traits, typename U>
    basic_password<CharT, Traits>::size_type erase(basic_password<CharT, Traits> & p, const U & value)
    {
        auto it = std::remove(p.begin(), p.end(), value);
        auto r = p.end() - it;
        p.erase(it, p.end());
        return r;
    }
    template <typename CharT, typename Traits, typename Pred>
    basic_password<CharT, Traits>::size_type erase_if(basic_password<CharT, Traits> & p, Pred pred)
    {
        auto it = std::remove_if(p.begin(), p.end(), pred);
        auto r = p.end() - it;
        p.erase(it, p.end());
        return r;
    }

    template <typename CharT, typename Traits>
    std::basic_istream<CharT, Traits> & getline(std::basic_istream<CharT, Traits> & input, basic_password<CharT, Traits> & p, CharT delim)
    {
        p.clear();

        static const std::size_t SIZE = 1024;
        CharT buffer[SIZE];

        while(input.getline(buffer, SIZE, delim) || input.gcount())
        {
            if(input.fail()) // No delimiter found but still data left in the stream (i.e. buffer is too small)
            {
                p.append(buffer, input.gcount());
                input.clear(input.rdstate() & ~std::ios_base::failbit);
            }
            else // Delimiter found (or input.eof())
            {
                p.append(buffer); // buffer is null-terminated in the place of the delimiter (or at the end if input.eof())
                break;
            }
        }

        return input;
    }
    template <typename CharT, typename Traits>
    std::basic_istream<CharT, Traits> & getline(std::basic_istream<CharT, Traits> && input, basic_password<CharT, Traits> & p, CharT delim)
    {
        return getline(input, p, delim); // Call the lvalue overload 
    }
    template <typename CharT, typename Traits>
    std::basic_istream<CharT, Traits> & getline(std::basic_istream<CharT, Traits> & input, basic_password<CharT, Traits> & p)
    {
        return getline(input, p, input.widen('\n'));
    }
    template <typename CharT, typename Traits>
    std::basic_istream<CharT, Traits> & getline(std::basic_istream<CharT, Traits> && input, basic_password<CharT, Traits> & p)
    {
        return getline(static_cast<std::basic_istream<CharT, Traits> &&>(input), p, input.widen('\n'));
    }

    using password = basic_password<char>;
}

#endif // MERLIN_BASIC_PASSWORD_HPP