/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParseHttp.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/04 15:02:13 by marvin            #+#    #+#             */
/*   Updated: 2024/12/04 15:02:13 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSEHTTP_HPP
# define PARSEHTTP_HPP
# include "webserv.hpp"
# include "ConfigParser.hpp"
# include "ClientRequest.hpp"

class	ParseHttp
{
	private:

	public:
		ParseHttp();
		~ParseHttp();
		ParseHttp(const ParseHttp &cp);
		ParseHttp& operator= (const ParseHttp &cp);

		int	fromSTRtoINT(std::string str);
		std::vector<int>	getListenVec(HttpBlock fileConfig, setOfRuleHTTP &rules);
		void	getDirRules(std::vector<Directive> dir,setOfRuleHTTP &rules, bool k);
		void	printSetOfRuleHTTP(setOfRuleHTTP config);
};

#endif
