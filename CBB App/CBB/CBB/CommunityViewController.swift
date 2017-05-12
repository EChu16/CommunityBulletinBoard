//
//  CommunityViewController.swift
//  CBB
//
//  Created by Erich Chu on 4/21/17.
//  Copyright © 2017 Erich Chu. All rights reserved.
//

import UIKit
import Alamofire
import SwiftyJSON

class CommunityViewController: UIViewController, UITableViewDelegate, UITableViewDataSource, UISearchBarDelegate {
    
    @IBOutlet weak var communityTable: UITableView!
    @IBOutlet weak var menuButton: UIBarButtonItem!
    @IBOutlet weak var rightBarItem: UIBarButtonItem!
    @IBOutlet weak var searchbar: UISearchBar!
    
    var user_name = ""
    var user_id = ""
    
    var communityBoards = [Dictionary<String,String>]()
    
    override func viewDidLoad() {
        super.viewDidLoad()
        searchbar.delegate = self
        setupMenu()
        retrieveAllBoards(search_name:"")
    }
    
    override func viewWillAppear(_ animated: Bool) {
        self.navigationItem.title = "Communities"
        self.rightBarItem.image = UIImage(named: "add-board-icon")
    }
    
    func searchBarSearchButtonClicked(_ searchBar: UISearchBar) {
        retrieveAllBoards(search_name:searchBar.text!)
    }
    
    func searchBarCancelButtonClicked(_ searchBar: UISearchBar) {
        print ("hit")
        retrieveAllBoards(search_name: "")
    }
    
    func setupMenu() {
        if (revealViewController() != nil) {
            menuButton.target = revealViewController()
            menuButton.action = #selector(SWRevealViewController.revealToggle(_:))
            revealViewController().rearViewRevealWidth = 275
            
            view.addGestureRecognizer(self.revealViewController().panGestureRecognizer())
        }
        customNavBar()
    }
    
    func customNavBar() {
        let navImg = #imageLiteral(resourceName: "nav-bar")
        
        self.navigationController?.navigationBar.setBackgroundImage(navImg, for: .default)
        
        navigationController?.navigationBar.tintColor = UIColor(colorLiteralRed: 1.0, green: 1.0, blue: 1.0, alpha: 1.0)
        navigationController?.navigationBar.titleTextAttributes = [NSForegroundColorAttributeName: UIColor.white, NSFontAttributeName: UIFont(name: "Aleo-Bold", size: 20.0)]

        
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    @IBAction func addBoardBtn(_ sender: UIBarButtonItem) {
        let revealViewController:SWRevealViewController = self.revealViewController()
        let mainStoryboard:UIStoryboard = UIStoryboard(name: "Main", bundle: nil)
        let desController = mainStoryboard.instantiateViewController(withIdentifier: "AddBoardViewController") as! AddBoardViewController
        desController.user_name = self.user_name
        desController.user_id = self.user_id
        let newFrontViewController = UINavigationController.init(rootViewController:desController)
        revealViewController.pushFrontViewController(newFrontViewController, animated: true)
    }
    
    func retrieveAllBoards(search_name: String) {
        // Flush out any existing boards for now
        communityBoards.removeAll()
        // create post request
        let parameters: Parameters = [
            "search_name": search_name,
        ]
        let url = "https://code.engineering.nyu.edu:8080/cbb/search"
        Alamofire.request(url, method: .post, parameters: parameters, encoding: JSONEncoding.default).responseJSON { response in
            if (response.response?.statusCode == nil) {
                print("Couldn't connect to server.")
            }
            else if (response.response?.statusCode==400) {
                print ("hit")
                debugPrint(String(data: response.data!, encoding: String.Encoding.utf8))
                print(response)
            }
            else if (response.response?.statusCode==200) {
                guard let object = response.result.value else {
                    print("Error")
                    return
                }
                let json = JSON(object)
                var counter : Int = 1;
                if let allBoards = json.array {
                    for board in allBoards {
                        let community_name = board["community_name"].string!
                        let num = String(counter) + "."
                        let dict = ["num": num, "community_name": community_name] as[String: String]
                        self.communityBoards.append(dict)
                        counter += 1
                    }
                }
                self.communityTable.reloadData()
            }
        }
    }
    
    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return communityBoards.count
    }
    
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        var cell = tableView.dequeueReusableCell(withIdentifier: "CommunityTableViewCell") as! CommunityTableViewCell
        cell.boardNum.text! = communityBoards[indexPath.row]["num"]!
        cell.boardName.text! = communityBoards[indexPath.row]["community_name"]!
        if (indexPath.row % 2 == 1) {
            cell.cellView.backgroundColor = UIColor(colorLiteralRed: 245.0/255.0, green: 230.0/255.0, blue: 203.0/255.0, alpha: 1.0)
        } 
        return cell
    }
    
    func tableView(_ tableView: UITableView, didSelectRowAt indexPath: IndexPath) {
        let revealViewController:SWRevealViewController = self.revealViewController()
        
        let cell:CommunityTableViewCell = tableView.cellForRow(at: indexPath) as! CommunityTableViewCell
        
        let mainStoryboard:UIStoryboard = UIStoryboard(name: "Main", bundle: nil)
        let desController = mainStoryboard.instantiateViewController(withIdentifier: "CBBViewController") as! CBBViewController
        desController.user_name = self.user_name
        desController.user_id = self.user_id
        let newFrontViewController = UINavigationController.init(rootViewController:desController)
        desController.community_name = cell.boardName.text!
        revealViewController.pushFrontViewController(newFrontViewController, animated: true)
    }

}
