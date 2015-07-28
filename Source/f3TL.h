#ifndef __F3TL_h__
#define __F3TL_h__

// minà»è„maxà»â∫Ç…êßå¿Ç∑ÇÈ
template<class T>
void Saturate(T min,T& num,T max){
	if (num>max) { num=max; return; }
	if (num<min) num=min;
};

// [start, end)Ç…Ç¬ÇﬂÇÈ
/*template<class T>
T Modulo(T start,T num,T end){
	if (start>end) { return Modulo(end, num, start); }
	if (start==end) { return start; }
	T dif = end - start, ret=num;
	while (ret<start) ret += dif;
	while (end<=ret) ret -= dif;
	return ret;
};*/

// minà»è„maxà»â∫Ç©Ç«Ç§Ç©
template<class T>
bool IsIn(T min,T num,T max){
	return min<=num && num<=max;
};

// fromÇtoÇ…stepÇæÇØãﬂÇ√ÇØÇÈ
template<class T>
void BringClose(T& from,T to,T step=1){
	if (step<0) step=-step;
	if (from>to) {
		from -= step;
		if (from<to) from=to;
	}else if (from<to) {
		from += step;
		if (from>to) from=to;
	}
};

// ì¡íËÇÃï∂éöóÒÇíuä∑(by S34 [www.s34.co.jp])
template<class E, class T, class A>
std::basic_string<E,T,A>
replace_all(
  const std::basic_string<E,T,A>& source,
  const std::basic_string<E,T,A>& pattern,
  const std::basic_string<E,T,A>& placement
  ) {
  std::basic_string<E,T,A> result;
  std::basic_string<E,T,A>::size_type pos_before = 0;
  std::basic_string<E,T,A>::size_type pos = 0;
  std::basic_string<E,T,A>::size_type len = pattern.size();
  while( ( pos = source.find( pattern, pos ) ) != std::string::npos ) {
    result.append(source, pos_before, pos - pos_before);
    result.append(placement);
    pos += len ;
    pos_before = pos ;
  }
  result.append(source, pos_before, source.size() - pos_before) ;
  return result;
}


#endif