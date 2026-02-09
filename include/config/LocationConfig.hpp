/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmaeda <kmaeda@student.42berlin.de>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/06 14:19:12 by kmaeda            #+#    #+#             */
/*   Updated: 2026/02/09 14:48:14 by kmaeda           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATIONCONFIG_HPP
#define LOCATIONCONFIG_HPP

#include <string>
#include <vector>

class LocationConfig {
	private:
		std::string path;
		std::vector<std::string> allowedMethods;
		std::string root;
		std::string indexFile;
	
	public:
		LocationConfig();

		// Getters
		std::string getPath() const;
		std::vector<std::string> getAllowedMethods() const;
		std::string getRoot() const;
		std::string getIndexFile() const;

		// Setters
		void setPath(const std::string& path);
		void addAllowedMethods(const std::string& method);
		void setRoot(const std::string& root);
		void setIndexFile(const std::string& index);

		bool isMethodAllowed(const std::string& method) const;
};

#endif
